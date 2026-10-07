#include "pch.h"
#include "CardFrame.h"
#include "MainFont.h"
#include "LocalizationManager.h"
#include "DX9GFUtils.h"
#include <algorithm>
#include <cmath>
#include <tuple>

namespace {
	// assets/cardtemplates.png: 48x16 cards in two columns. The left column holds 8 templates from
	// the top; the right column's first slot is the energy orb, so its templates start one row down.
	constexpr int TEMPLATE_W = 48;
	constexpr int TEMPLATE_H = 16;
	constexpr int LEFT_COLUMN_X = 0;
	constexpr int RIGHT_COLUMN_X = 48;
	constexpr int LEFT_COLUMN_COUNT = 8;

	// Cut points inside one template. Everything from x=11 to x=44 is the same column repeated (the
	// icon ends by x=11), so any column in that stretch can be the body; 16 stays clear of the blue
	// template's grid lines.
	constexpr int LEFT_CAP_W = 14;
	constexpr int RIGHT_CAP_W = 3;
	constexpr int BODY_SRC_X = 16;

	constexpr RECT ORB_SRC = { 52, 4, 60, 12 };
	constexpr int ORB_SIZE = 8;

	// Spacing, in sheet pixels.
	constexpr int NAME_COST_GAP = 3;
	constexpr int DIGIT_ORB_GAP = 1;
	constexpr int ORB_RIGHT_GAP = 1;

	// tools/build_cardpixel_font.py builds this font from the hand-drawn lettering the card sprites
	// used to have. Its cell is 9 design pixels, so size 18 is 2 screen pixels per design pixel -
	// its look at the battle scale of 2. Other scales stretch it by scale / FONT_NATIVE_SCALE.
	constexpr const wchar_t* CARD_FONT_NAME = L"CardPixel";
	constexpr const wchar_t* CARD_FONT_TOP_NAME = L"CardPixelTop";
	constexpr const wchar_t* CARD_FONT_BOTTOM_NAME = L"CardPixelBottom";
	constexpr int CARD_FONT_SIZE = 18;
	constexpr float FONT_NATIVE_SCALE = 2.f;
	// Where the font's cell starts, in sheet pixels from the top of the card. Capitals begin one
	// design pixel into the cell, so this puts them on rows 5-10 of the 16-row card like the old art.
	constexpr int TEXT_TOP = 4;

	// Name colours lifted from the hand-drawn strips this replaced, indexed like CardTemplate: the
	// upper half of the letters over the card's light band, the lower half over its dark one. The
	// cost digit was the same off-white, outlined in black, on every card.
	struct NameColors { D3DCOLOR top; D3DCOLOR bottom; };
	constexpr NameColors NAME_COLORS[] = {
		{ 0xFFFFFC40, 0xFFD6F264 }, // Red
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Magenta
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Rose
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Green
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Forest
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Cyan
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Periwinkle
		{ 0xFFFFFFFF, 0xFFDAE0EA }, // Steel
		{ 0xFFFFFFFF, 0xFFFFFFFF }, // Blue
		{ 0xFFFFFC40, 0xFFD6F264 }, // Orange
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Pine
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Chartreuse
		{ 0xFFF9A31B, 0xFFFA6A0A }, // Yellow
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Aqua
		{ 0xFF4AA1E8, 0xFF1062A5 }, // Sage
	};
	static_assert(sizeof(NAME_COLORS) / sizeof(NAME_COLORS[0]) == static_cast<size_t>(Demo::CardTemplate::Count),
		"one name colour per CardTemplate");
	constexpr D3DCOLOR COST_COLOR = 0xFFE3E6FF;
	// The "(_)" after the name was the same cool grey on every card.
	constexpr D3DCOLOR INPUT_COLOR = 0xFFB3B9D1;
	// A filled slot's "x" has to stand out from that grey: orange, except on the templates whose own
	// colours are orange or yellow. Cyan reads on orange but washes out against yellow, which wants
	// something dark - a saturated blue.
	constexpr D3DCOLOR FILLED_INPUT_COLOR = 0xFFFF9A1F;
	constexpr D3DCOLOR FILLED_INPUT_COLOR_ON_ORANGE = 0xFF22E6F0;
	constexpr D3DCOLOR FILLED_INPUT_COLOR_ON_YELLOW = 0xFF1B3FD6;
	constexpr D3DCOLOR OUTLINE_COLOR = 0xFF060608;

	// Component-wise multiply, the same way a sprite's colour modulates its texture.
	D3DCOLOR Modulate(D3DCOLOR a, D3DCOLOR b) {
		auto ch = [&](int shift) { return ((a >> shift) & 0xFF) * ((b >> shift) & 0xFF) / 255; };
		return D3DCOLOR_ARGB(ch(24), ch(16), ch(8), ch(0));
	}

	RECT TemplateRect(Demo::CardTemplate t) {
		int index = static_cast<int>(t);
		if (index < 0 || index >= static_cast<int>(Demo::CardTemplate::Count)) index = 0;
		if (index < LEFT_COLUMN_COUNT) {
			const int y = index * TEMPLATE_H;
			return { LEFT_COLUMN_X, y, LEFT_COLUMN_X + TEMPLATE_W, y + TEMPLATE_H };
		}
		const int y = (index - LEFT_COLUMN_COUNT + 1) * TEMPLATE_H;
		return { RIGHT_COLUMN_X, y, RIGHT_COLUMN_X + TEMPLATE_W, y + TEMPLATE_H };
	}
}

std::wstring Demo::CardDisplayNameFromSaveID(const std::string& saveID) {
	std::string id = saveID;
	const std::string suffix = "Card";
	if (id.size() > suffix.size() && id.compare(id.size() - suffix.size(), suffix.size(), suffix) == 0) {
		id.resize(id.size() - suffix.size());
	}
	return DX9GF::Utils::Utf8ToWide(id);
}

std::wstring Demo::CardInputSignature(size_t slots, size_t filled) {
	std::wstring signature = L"(";
	for (size_t i = 0; i < slots; ++i) {
		if (i > 0) signature += L',';
		signature += i < filled ? L'x' : L'_';
	}
	return signature + L")";
}

std::shared_ptr<Demo::CardFrame> Demo::CardFrame::Get(DX9GF::GraphicsDevice* graphicsDevice) {
	static std::weak_ptr<CardFrame> cache;
	auto frame = cache.lock();
	if (!frame || frame->graphicsDevice != graphicsDevice) {
		frame = std::make_shared<CardFrame>(graphicsDevice);
		cache = frame;
	}
	return frame;
}

Demo::CardFrame::CardFrame(DX9GF::GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {
	texture = std::make_shared<DX9GF::Texture>(graphicsDevice);
	texture->LoadTexture(L"assets/cardtemplates.png");
	sheet = std::make_shared<DX9GF::StaticSprite>(texture.get());

	font = std::make_shared<DX9GF::Font>(graphicsDevice, CARD_FONT_NAME, CARD_FONT_SIZE);
	fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
	topFont = std::make_shared<DX9GF::Font>(graphicsDevice, CARD_FONT_TOP_NAME, CARD_FONT_SIZE);
	topSprite = std::make_shared<DX9GF::FontSprite>(topFont.get());
	bottomFont = std::make_shared<DX9GF::Font>(graphicsDevice, CARD_FONT_BOTTOM_NAME, CARD_FONT_SIZE);
	bottomSprite = std::make_shared<DX9GF::FontSprite>(bottomFont.get());
	fontSprite->SetText(std::wstring(L"0"));
	digitWidth = static_cast<float>(fontSprite->GetWidth());
}

namespace {
	// Faces queued while collecting. Static because the collector is driven from DraggableManager::Draw,
	// which has no device to look a CardFrame up by; the frame that queued them draws them.
	bool g_collecting = false;
}

float Demo::CardFrame::TextWidth(const std::wstring& text) {
	auto it = widthCache.find(text);
	if (it != widthCache.end()) return it->second;
	// Names and signatures are a small closed set, but don't let an odd caller grow this forever.
	if (widthCache.size() > 4096) widthCache.clear();
	fontSprite->SetText(text);
	const float w = static_cast<float>(fontSprite->GetWidth());
	widthCache.emplace(text, w);
	return w;
}

float Demo::CardFrame::MeasureWidth(const std::wstring& name, const std::wstring& inputs, float scale) {
	const float textScale = scale / FONT_NATIVE_SCALE;
	const float textW = TextWidth(name + inputs) * textScale;
	const float digitW = digitWidth * textScale;
	const float sheetW = LEFT_CAP_W + NAME_COST_GAP + DIGIT_ORB_GAP + ORB_SIZE + ORB_RIGHT_GAP + RIGHT_CAP_W;
	// Whole sheet pixels, so the body column scales by an exact multiple.
	return std::ceil((sheetW * scale + textW + digitW) / scale) * scale;
}

void Demo::CardFrame::DrawSlice(DX9GF::StaticSprite& sprite, const DX9GF::Camera& camera, unsigned long long deltaTime,
	RECT src, float x, float y, float scaleX, float scaleY, D3DCOLOR tint) {
	sprite.SetSrcRect(src);
	sprite.SetColor(tint);
	sprite.SetPosition(x, y);
	sprite.SetScale(scaleX, scaleY);
	sprite.Draw(camera, deltaTime);
}

void Demo::CardFrame::DrawLabel(DX9GF::FontSprite& sprite, const DX9GF::Camera& camera, unsigned long long deltaTime,
	const std::wstring& text, float x, float y, float scale, D3DCOLOR color, bool outlined, D3DCOLOR tint) {
	sprite.SetText(text);
	sprite.SetColor(color);
	sprite.SetOutline(outlined, Modulate(OUTLINE_COLOR, tint), scale);
	sprite.SetScale(scale / FONT_NATIVE_SCALE, scale / FONT_NATIVE_SCALE);
	sprite.SetPosition(x, y);
	sprite.Draw(camera, deltaTime);
}

void Demo::CardFrame::DrawGroup(unsigned long long deltaTime, const Entry* entries, size_t count) {
	if (count == 0) return;
	const DX9GF::Camera& camera = *entries[0].camera;

	struct Layout {
		float width;
		float leftW;
		float rightW;
		float textScale;
		float textY;
	};
	std::vector<Layout> layouts;
	layouts.reserve(count);
	for (size_t i = 0; i < count; ++i) {
		const Entry& e = entries[i];
		Layout l;
		l.width = MeasureWidth(e.name, e.inputs, e.scale);
		l.leftW = LEFT_CAP_W * e.scale;
		l.rightW = RIGHT_CAP_W * e.scale;
		l.textScale = e.scale / FONT_NATIVE_SCALE;
		l.textY = e.y + TEXT_TOP * e.scale;
		layouts.push_back(l);
	}

	// Pass 1: every card's frame and energy orb, in one sprite batch.
	sheet->Begin();
	for (size_t i = 0; i < count; ++i) {
		const Entry& e = entries[i];
		const Layout& l = layouts[i];
		const RECT t = TemplateRect(e.cardTemplate);
		const float bodyW = l.width - l.leftW - l.rightW;
		DrawSlice(*sheet, camera, deltaTime, { t.left, t.top, t.left + LEFT_CAP_W, t.bottom },
			e.x, e.y, e.scale, e.scale, e.tint);
		// The body is one source column stretched across; its width in sheet pixels is the scale factor.
		DrawSlice(*sheet, camera, deltaTime, { t.left + BODY_SRC_X, t.top, t.left + BODY_SRC_X + 1, t.bottom },
			e.x + l.leftW, e.y, bodyW, e.scale, e.tint);
		DrawSlice(*sheet, camera, deltaTime, { t.right - RIGHT_CAP_W, t.top, t.right, t.bottom },
			e.x + l.width - l.rightW, e.y, e.scale, e.scale, e.tint);
		const float orbX = e.x + l.width - l.rightW - (ORB_RIGHT_GAP + ORB_SIZE) * e.scale;
		DrawSlice(*sheet, camera, deltaTime, ORB_SRC, orbX, e.y + (Height(e.scale) - ORB_SIZE * e.scale) / 2.f,
			e.scale, e.scale, e.tint);
	}
	sheet->End();

	// Passes 2 and 3: each font only has its half of every letter, so together they make the whole name.
	topSprite->Begin();
	for (size_t i = 0; i < count; ++i) {
		const Entry& e = entries[i];
		const NameColors& nameColors = NAME_COLORS[static_cast<int>(e.cardTemplate)];
		DrawLabel(*topSprite, camera, deltaTime, e.name, e.x + layouts[i].leftW, layouts[i].textY, e.scale,
			Modulate(nameColors.top, e.tint), false, e.tint);
	}
	topSprite->End();
	bottomSprite->Begin();
	for (size_t i = 0; i < count; ++i) {
		const Entry& e = entries[i];
		const NameColors& nameColors = NAME_COLORS[static_cast<int>(e.cardTemplate)];
		DrawLabel(*bottomSprite, camera, deltaTime, e.name, e.x + layouts[i].leftW, layouts[i].textY, e.scale,
			Modulate(nameColors.bottom, e.tint), false, e.tint);
	}
	bottomSprite->End();

	// Pass 4: input signatures and cost digits.
	fontSprite->Begin();
	for (size_t i = 0; i < count; ++i) {
		const Entry& e = entries[i];
		const Layout& l = layouts[i];
		const float nameW = TextWidth(e.name) * l.textScale;
		// The signature is drawn in runs so each "x" can take its own colour. Glyph advances add up, so a
		// run's start is just the measured width of everything before it.
		D3DCOLOR filledColor = FILLED_INPUT_COLOR;
		if (e.cardTemplate == CardTemplate::Orange) filledColor = FILLED_INPUT_COLOR_ON_ORANGE;
		else if (e.cardTemplate == CardTemplate::Yellow) filledColor = FILLED_INPUT_COLOR_ON_YELLOW;
		float runX = e.x + l.leftW + nameW;
		size_t pos = 0;
		while (pos < e.inputs.size()) {
			size_t end = e.inputs[pos] == L'x' ? pos + 1 : e.inputs.find(L'x', pos);
			if (end == std::wstring::npos) end = e.inputs.size();
			const std::wstring run = e.inputs.substr(pos, end - pos);
			const bool filled = e.inputs[pos] == L'x';
			DrawLabel(*fontSprite, camera, deltaTime, run, runX, l.textY, e.scale,
				Modulate(filled ? filledColor : INPUT_COLOR, e.tint), false, e.tint);
			runX += TextWidth(run) * l.textScale;
			pos = end;
		}

		const float orbX = e.x + l.width - l.rightW - (ORB_RIGHT_GAP + ORB_SIZE) * e.scale;
		const float digitX = orbX - DIGIT_ORB_GAP * e.scale - digitWidth * l.textScale;
		DrawLabel(*fontSprite, camera, deltaTime, std::to_wstring(e.cost), digitX, l.textY, e.scale,
			Modulate(COST_COLOR, e.tint), true, e.tint);
	}
	fontSprite->End();
}

void Demo::CardFrame::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime, float x, float y,
	CardTemplate cardTemplate, const std::wstring& name, const std::wstring& inputs, size_t cost, float scale, D3DCOLOR tint) {
	// Anything queued before this face was drawn before it, so it has to land first.
	FlushCollected();
	const Entry entry{ &camera, x, y, cardTemplate, name, inputs, cost, scale, tint, false, RECT{} };
	DrawGroup(deltaTime, &entry, 1);
}

void Demo::CardFrame::BeginCollect() {
	g_collecting = true;
}

void Demo::CardFrame::EndCollect() {
	FlushCollected();
	g_collecting = false;
}

bool Demo::CardFrame::IsCollecting() {
	return g_collecting;
}

std::vector<Demo::CardFrame::Entry> Demo::CardFrame::pending;
Demo::CardFrame* Demo::CardFrame::pendingOwner = nullptr;

void Demo::CardFrame::Queue(const DX9GF::Camera& camera, float x, float y,
	CardTemplate cardTemplate, std::wstring name, std::wstring inputs, size_t cost, float scale, D3DCOLOR tint, const RECT* scissor) {
	// Faces from another device's frame can't share a batch.
	if (pendingOwner && pendingOwner != this) FlushCollected();
	pendingOwner = this;
	pending.push_back(Entry{ &camera, x, y, cardTemplate, std::move(name), std::move(inputs), cost, scale, tint,
		scissor != nullptr, scissor ? *scissor : RECT{} });
}

void Demo::CardFrame::FlushCollected() {
	if (pending.empty() || !pendingOwner) return;
	CardFrame* owner = pendingOwner;
	std::vector<Entry> entries = std::move(pending);
	pending.clear();
	pendingOwner = nullptr;
	owner->FlushPending(entries);
}

void Demo::CardFrame::FlushPending(std::vector<Entry>& entries) {
	// Group faces that can share one batch. A stable sort keeps submission order inside a group; faces
	// only change order relative to faces with a different camera or scissor, which don't overlap.
	std::stable_sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
		if (a.camera != b.camera) return a.camera < b.camera;
		if (a.cropped != b.cropped) return a.cropped < b.cropped;
		if (!a.cropped) return false;
		return std::tie(a.scissor.left, a.scissor.top, a.scissor.right, a.scissor.bottom)
			< std::tie(b.scissor.left, b.scissor.top, b.scissor.right, b.scissor.bottom);
		});

	auto sameGroup = [](const Entry& a, const Entry& b) {
		return a.camera == b.camera && a.cropped == b.cropped
			&& (!a.cropped || (a.scissor.left == b.scissor.left && a.scissor.top == b.scissor.top
				&& a.scissor.right == b.scissor.right && a.scissor.bottom == b.scissor.bottom));
		};

	size_t begin = 0;
	while (begin < entries.size()) {
		size_t end = begin + 1;
		while (end < entries.size() && sameGroup(entries[begin], entries[end])) ++end;
		if (entries[begin].cropped) {
			graphicsDevice->SetScissorRect(entries[begin].scissor);
			graphicsDevice->SetScissorTest(true);
		}
		DrawGroup(0, entries.data() + begin, end - begin);
		if (entries[begin].cropped) {
			graphicsDevice->SetScissorTest(false);
		}
		begin = end;
	}
}

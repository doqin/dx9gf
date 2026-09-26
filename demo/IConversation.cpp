#include "pch.h"
#include "IConversation.h"
#include <DX9GFInputManager.h>
#include <DX9GFApplication.h>
#include "DX9GFAudioManager.h"
#include "SettingsManager.h"
#include <cmath>
#include <algorithm>
Demo::IConversation::IConversation(std::shared_ptr<DX9GF::FontSprite> fontSprite, int screenWidth, int screenHeight)
	: fontSprite(fontSprite), virtualWidth((float)screenWidth), virtualHeight((float)screenHeight)
{
}

void Demo::IConversation::AddLine(const DialogueLine& line)
{
	if (dialogueQueue.empty()) {
		isTyping = true; 
	}
	dialogueQueue.push(line);
}

void Demo::IConversation::Execute(unsigned long long deltaTime)
{
	if (dialogueQueue.empty()) {
		MarkFinished();
		return;
	}

	const auto& currentLine = dialogueQueue.front();

	if (isTyping) {
		timer += deltaTime;
		if (timer >= MS_PER_CHAR) {
			timer = 0;
			if (currentCharIndex < currentLine.content.length()) {
				currentCharIndex++;
				displayedContent = currentLine.content.substr(0, currentCharIndex);
				//skip space
				if (currentLine.content[currentCharIndex - 1] != L' ') {
					DX9GF::AudioManager::GetInstance()->Play(currentLine.voiceClip.has_value() ? currentLine.voiceClip.value() : "bleep5");
				}
			}
			else {
				isTyping = false;
			}
		}
	}

	auto input = DX9GF::InputManager::GetInstance();
	const int keyAccept = SettingsManager::GetInstance()->GetKeybind("ACCEPT");
	bool advance = false;
	if (input->MouseDown(DX9GF::InputManager::MouseButton::Left)) {
		input->ConsumeMouseButton(DX9GF::InputManager::MouseButton::Left);
		advance = true;
	}
	else if (input->KeyDown(keyAccept)) {
		input->ConsumeKey(keyAccept);
		advance = true;
	}
	if (advance) {
		if (isTyping) {
			displayedContent = currentLine.content;
			currentCharIndex = currentLine.content.length();
			isTyping = false;
		}
		else {
			dialogueQueue.pop();
			if (dialogueQueue.empty()) {
				MarkFinished();
			}
			else {
				ResetAnimation();
			}
		}
	}
}

void Demo::IConversation::Draw(DX9GF::GraphicsDevice* gd, DX9GF::Camera* uiCamera, unsigned long long deltaTime)
{
	if (dialogueQueue.empty() || IsFinished() || !gd || !uiCamera) return;

	const auto& currentLine = dialogueQueue.front();

	float leftEdge = -virtualWidth / 2.0f;
	float rightEdge = virtualWidth / 2.0f;
	float bottomEdge = virtualHeight / 2.0f;

	if (currentLine.left.has_value() && currentLine.left.value()) {
		auto sprite = currentLine.left.value();
		sprite->SetPosition(leftEdge + 100.0f, bottomEdge - 180.0f);
		sprite->Begin();
		sprite->Draw(*uiCamera, deltaTime);
		sprite->End();
	}

	if (currentLine.right.has_value() && currentLine.right.value()) {
		auto sprite = currentLine.right.value();
		sprite->SetPosition(rightEdge - 100.0f, bottomEdge - 180.0f);
		sprite->Begin();
		sprite->Draw(*uiCamera, deltaTime);
		sprite->End();
	}

	const float boxWidth = virtualWidth - 40.0f;
	const float textPaddingX = 20.0f;
	const float contentTop = 56.0f;
	const float contentBottomPadding = 24.0f;

	float contentHeight = 0.0f;
	if (fontSprite) {
		fontSprite->SetScale(1.f, 1.f);
		if (wrappedSource != currentLine.content) {
			wrappedSource = currentLine.content;
			wrappedContent = WrapToWidth(currentLine.content, boxWidth - textPaddingX * 2.0f);
		}
		// Sized from the whole line, not the typed-so-far part, so the box doesn't grow mid-line.
		fontSprite->SetText(wrappedContent);
		contentHeight = static_cast<float>(fontSprite->GetHeight());
	}

	float boxHeight = (std::max)(120.0f, contentTop + contentHeight + contentBottomPadding);
	float boxX = leftEdge + 20.0f;
	float boxY = bottomEdge - boxHeight - 20.0f;

	gd->DrawRectangle(
		*uiCamera,
		boxX, boxY, boxWidth, boxHeight,
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0xFFE0E0E0, true
	);
	gd->DrawRectangle(
		*uiCamera,
		boxX, boxY, boxWidth, boxHeight,
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0xFF000000, false
	);

	// Advance indicator: a small down-pointing triangle bobbing at the bottom center
	// of the box once the line has fully revealed.
	if (!isTyping) {
		const float triangleWidth = 16.0f;
		const float triangleHeight = 10.0f;
		const float bounce = std::sin(static_cast<float>(GetTickCount64()) * 0.006f) * 3.0f;
		const float triangleTopY = boxY + boxHeight - triangleHeight - 10.0f + bounce;
		gd->DrawTriangle(*uiCamera, boxX + boxWidth / 2.0f, triangleTopY, triangleWidth, triangleHeight, 0xFF000000, true);
	}

	if (fontSprite) {
		fontSprite->Begin();
		fontSprite->SetScale(1.5f, 1.5f); // whole multiple of the font's pixel grid, so strokes stay even
		fontSprite->SetPosition(boxX + textPaddingX, boxY + 10.0f);
		fontSprite->SetColor(0xFFFFFFFF);
		fontSprite->SetOutline(true, 0xFF000000, 2.f);
		fontSprite->SetText(std::wstring(currentLine.name));
		fontSprite->Draw(*uiCamera, deltaTime);

		fontSprite->SetOutline(false);
		fontSprite->SetScale(1.f, 1.f);
		fontSprite->SetPosition(boxX + textPaddingX, boxY + contentTop);
		fontSprite->SetColor(0xFF000000);
		fontSprite->SetText(wrappedContent.substr(0, (std::min)(displayedContent.size(), wrappedContent.size())));
		fontSprite->Draw(*uiCamera, deltaTime);

		fontSprite->End();
	}
}

std::wstring Demo::IConversation::WrapToWidth(const std::wstring& text, float maxWidth)
{
	std::wstring out = text;
	size_t lineStart = 0;
	size_t lastSpace = std::wstring::npos;
	for (size_t i = 0; i <= out.size(); ++i) {
		const wchar_t c = i == out.size() ? L'\n' : out[i];
		if (c != L' ' && c != L'\n') continue;

		fontSprite->SetText(out.substr(lineStart, i - lineStart));
		if (fontSprite->GetWidth() > maxWidth && lastSpace != std::wstring::npos) {
			out[lastSpace] = L'\n';
			lineStart = lastSpace + 1;
		}

		if (c == L'\n') {
			lineStart = i + 1;
			lastSpace = std::wstring::npos;
		}
		else {
			lastSpace = i;
		}
	}
	return out;
}

void Demo::IConversation::ResetAnimation() {
	displayedContent = L"";
	currentCharIndex = 0;
	timer = 0;
	isTyping = true;
}
#include "pch.h"
#include "SketchyGuyNPC.h"
#include "LocalizationManager.h"

namespace Demo {
	SketchyGuyNPC::SketchyGuyNPC(std::weak_ptr<DX9GF::TransformManager> tm, float x, float y)
		: INPC(tm, x, y) {
	}

	void SketchyGuyNPC::Init(DX9GF::GraphicsDevice* gd, DX9GF::Camera* camera, std::shared_ptr<Player> p,
		std::shared_ptr<DX9GF::ColliderManager> cm, std::shared_ptr<DX9GF::Font> font,
		std::shared_ptr<DX9GF::CommandBuffer> drawBuffer)
	{
		INPC::Init(gd, camera, p, cm, font, drawBuffer);
		this->gd = gd;
		this->colliderManager = cm;

		collider = std::make_shared<DX9GF::RectangleCollider>(
			transformManager, 20.f, 10.f, GetWorldX(), GetWorldY() + 8.f);
		collider->SetOriginCenter();
		cm->Add(collider);

		spritesheet = std::make_shared<DX9GF::Texture>(gd);
		spritesheet->LoadTexture(L"assets/sketchyguy-Sheet.png");
		// 64x32 sheet -> two 32x32 idle frames.
		sprite = std::make_shared<DX9GF::AnimatedSprite>(
			spritesheet.get(), DX9GF::Utils::CreateRectsHorizontal(0, 0, 32, 32, 2), 4, true);
		sprite->SetOrigin(16.f, 24.f);
		sprite->SetPosition(GetWorldX(), GetWorldY());

		// Intro script - hardcoded so every scene that spawns him gets the same lines.
		AddLine(Tr(L"???"), Tr(L"Psst. Down here. Don't make it look like we're talking."));
		AddLine(Tr(L"???"), Tr(L"You didn't see me, I was never here, and this exchange has already\nbeen garbage-collected. We clear?"));
		AddLine(Tr(L"Player"), Tr(L"...Sure. What are you selling?"));
		AddLine(Tr(L"???"), Tr(L"Sealed packs. Contents sourced through channels I'm advised not to\ndescribe out loud."));
		AddLine(Tr(L"???"), Tr(L"Could be a Legendary. Could be five copies of Strike.\nThat's the thrill. That's what you're paying for."));
		AddLine(Tr(L"Player"), Tr(L"I already regret this. Catch me next time you're skulking around."));
	}

	void SketchyGuyNPC::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime)
	{
		sprite->Begin();
		sprite->Draw(camera, deltaTime);
		sprite->End();

		INPC::Draw(camera, deltaTime);
	}

	std::wstring SketchyGuyNPC::GetBuyPromptTitle() const
	{
		return Tr(L"SEALED PACK");
	}

	std::wstring SketchyGuyNPC::GetBuyPromptText(int cost) const
	{
		if (cost <= 0) {
			return Tr(L"First one's on the house. Five cards, sealed.\nNo receipt, no returns. Want it?");
		}
		return Tr(L"One sealed pack. Five cards. ") + std::to_wstring(cost) + Tr(L" Gold.\nNo questions, no refunds.");
	}

	std::wstring SketchyGuyNPC::GetBuyConfirmLabel(int cost) const
	{
		return cost <= 0 ? Tr(L"Take it(Y)") : Tr(L"Buy(Y)");
	}
}

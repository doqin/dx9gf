#include "pch.h"
#include "IConversation.h"
#include <DX9GFInputManager.h>
#include <DX9GFApplication.h>

Demo::IConversation::IConversation(std::shared_ptr<DX9GF::FontSprite> fontSprite, int screenWidth, int screenHeight)
	: fontSprite(fontSprite), fixedCamera(screenWidth, screenHeight)
{
	fixedCamera.SetPosition(screenWidth / 2.0f, screenHeight / 2.0f);
	fixedCamera.Update();
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
			}
			else {
				isTyping = false;
			}
		}
	}

	auto input = DX9GF::InputManager::GetInstance();
	if (input->MouseDown(DX9GF::InputManager::MouseButton::Left)) {
		input->ConsumeMouseButton(DX9GF::InputManager::MouseButton::Left);

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

void Demo::IConversation::Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime)
{
	if (dialogueQueue.empty() || IsFinished() || !gd) return;

	const auto& currentLine = dialogueQueue.front();

	auto [screenW, screenH] = fixedCamera.GetScreenResolution();
	float fScreenW = static_cast<float>(screenW);
	float fScreenH = static_cast<float>(screenH);

	if (currentLine.left.has_value() && currentLine.left.value()) {
		auto sprite = currentLine.left.value();
		sprite->SetPosition(100.0f, fScreenH - 180.0f);
		sprite->Begin();
		sprite->Draw(fixedCamera, deltaTime);
		sprite->End();
	}

	if (currentLine.right.has_value() && currentLine.right.value()) {
		auto sprite = currentLine.right.value();
		sprite->SetPosition(fScreenW - 100.0f, fScreenH - 180.0f);
		sprite->Begin();
		sprite->Draw(fixedCamera, deltaTime);
		sprite->End();
	}

	float boxWidth = fScreenW - 40.0f;
	float boxHeight = 120.0f;
	float boxX = 20.0f;
	float boxY = fScreenH - boxHeight - 20.0f;

	gd->DrawRectangle(
		fixedCamera,
		boxX, boxY, boxWidth, boxHeight,
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0xDD8B4513, true
	);

	if (fontSprite) {
		fontSprite->Begin();

		fontSprite->SetPosition(boxX + 20.0f, boxY + 10.0f);
		fontSprite->SetColor(0xFFFFD700);
		fontSprite->SetText(std::wstring(currentLine.name));
		fontSprite->Draw(fixedCamera, deltaTime);
		fontSprite->SetPosition(boxX + 20.0f, boxY + 50.0f);
		fontSprite->SetColor(0xFFFFFFFF);
		fontSprite->SetText(std::wstring(displayedContent));
		fontSprite->Draw(fixedCamera, deltaTime);

		fontSprite->End();
	}
}

void Demo::IConversation::ResetAnimation() {
	displayedContent = L"";
	currentCharIndex = 0;
	timer = 0;
	isTyping = true;
}
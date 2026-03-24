#pragma once
#include <DX9GFExtras\DX9GFICommand.h>
#include <DX9GFSprites.h>
#include <DX9GFGraphicsDevice.h>
#include <DX9GFCamera.h>
#include <queue>
#include "DialogueLine.h"

namespace Demo {
	class IConversation : public DX9GF::ICommand {
	private:
		std::queue<DialogueLine> dialogueQueue;
		std::shared_ptr<DX9GF::FontSprite> fontSprite;
		DX9GF::Camera fixedCamera; 
	public:
		IConversation(std::shared_ptr<DX9GF::FontSprite> fontSprite, int screenWidth, int screenHeight);

		void AddLine(const DialogueLine& line);

		void Execute(unsigned long long deltaTime) override;

		void Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime);
	};
}

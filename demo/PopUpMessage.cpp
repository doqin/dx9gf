#include "pch.h"
#include "PopUpMessage.h"

void Demo::PopUpMessage::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera)
{
	this->graphicsDevice = graphicsDevice;
	this->camera = camera;
	font = std::make_shared<DX9GF::Font>(graphicsDevice, L"StatusPlz", 16);
	fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
	fontSprite->SetColor(0xFF000000);
}

void Demo::PopUpMessage::QueueMessage(DX9GF::CommandBuffer* commandBuffer, std::wstring message)
{
	fontSprite->SetText(std::move(message));
	auto fontHeight = fontSprite->GetHeight();
	auto app = DX9GF::Application::GetInstance();
	commandBuffer->PushCommand(std::make_shared<DX9GF::SetPositionCommand>(shared_from_this(), 0, -static_cast<float>(app->GetScreenHeight())));
	commandBuffer->PushCommand(std::make_shared<DX9GF::CustomCommand>([this](std::function<void(void)> markFinished) {
		isDrawing = true;
		markFinished();
		}));
	commandBuffer->PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), 0, -static_cast<float>(app->GetScreenHeight()) / 2.f + fontHeight + 20.f, 0.5f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
	commandBuffer->PushCommand(std::make_shared<DX9GF::DelayCommand>(1.f));
	commandBuffer->PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), 0, -static_cast<float>(app->GetScreenHeight()), 0.5f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
	commandBuffer->PushCommand(std::make_shared<DX9GF::CustomCommand>([this](std::function<void(void)> markFinished) {
		isDrawing = false;
		markFinished();
	}));
}

void Demo::PopUpMessage::Draw(unsigned long long deltaTime)
{
	if (isDrawing) {
		auto width = fontSprite->GetWidth();
		auto height = fontSprite->GetHeight();
		fontSprite->SetPosition(GetWorldX() - width / 2.f, GetWorldY() - height / 2.f);
		fontSprite->Begin();
		fontSprite->Draw(*camera, deltaTime);
		fontSprite->End();
	}
}

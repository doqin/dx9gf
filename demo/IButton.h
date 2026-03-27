#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <memory>
#include <functional>
namespace Demo
{
	class IButton : public DX9GF::IGameObject
	{
	public:
		//button state to change its appearance, have to set it public for scene to use
		enum class ButtonState { IDLE, HOVER, CLICKED, DISABLED, LISTENING };
	protected:
		ButtonState currentState;

		float width;
		float height;

		//to reuse component as buttons may have different frame counts.
		int frameCount;

		std::shared_ptr<DX9GF::RectangleTrigger> trigger;
		std::function<void(DX9GF::ITrigger*)> callback;

	public:

		IButton(std::shared_ptr<DX9GF::TransformManager> tm, float x, float y, float w, float h, int frames = 3)
			: IGameObject(tm, x, y)
		{
			this->width = w;
			this->height = h;
			this->currentState = ButtonState::IDLE;
			this->frameCount = frames;
		}

		virtual void Init(DX9GF::Camera* uiCamera) = 0;
		//abstract function to determine whether the button displays text or an icon
		virtual void Draw(DX9GF::Camera* camera, DX9GF::GraphicsDevice* gd, unsigned long long deltaTime) = 0;


		virtual void SetOnReleaseLeft(std::function<void(DX9GF::ITrigger*)> cb)
		{
			//save the button's feature
			this->callback = cb;

			if (this->trigger && this->callback)
			{
				this->trigger->SetOnReleaseLeft(this->callback);
			}
		}

		//check if the cursor is hovering over the button.
		virtual void Update(unsigned long long deltaTime)
		{
			if (!this->trigger)
				return;

			//update trigger
			this->trigger->Update(deltaTime);

			//block when the button is not ready
			if (this->currentState == ButtonState::LISTENING|| this->currentState == ButtonState::DISABLED) {
				return;
			}

			//change button state
			if (this->trigger->IsHovering(deltaTime))
			{
				if (this->trigger->IsHeldLeft(deltaTime)) {
					this->currentState = ButtonState::CLICKED;
				}
				else {
					this->currentState = ButtonState::HOVER;
				}
			}
			else
			{
				this->currentState = ButtonState::IDLE;
			}
		}

		std::shared_ptr<DX9GF::RectangleTrigger> GetTrigger() { return this->trigger; }
		void SetState(ButtonState state) { this->currentState = state; }
		float GetWidth() { return this->width; }
		float GetHeight() { return this->height; }
	};
}

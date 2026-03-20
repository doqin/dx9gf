#pragma once
#include "DX9GFRectangleTrigger.h"
#include <memory>
#include <functional>
#include <DX9GF.h>
namespace DX9GF
{
	class IButton : public IGameObject
	{
	protected:
		//button state to change its appearance,...
		enum class ButtonState { IDLE, HOVER, CLICKED, DISABLED };
		ButtonState currentState;
		float width;
		float height;
		std::shared_ptr<RectangleTrigger> trigger;

		std::function<void(ITrigger*)> callback;

	public:

		IButton(std::shared_ptr<TransformManager> tm, float x, float y, float w, float h)
			: IGameObject(tm, x, y)
		{
			this->width = w;
			this->height = h;
			this->currentState = ButtonState::IDLE;
		}

		virtual void Init(DX9GF::Camera* uiCamera) = 0;
		//abstract function to determine whether the button displays text or an icon
		virtual void Draw(Camera* camera, GraphicsDevice* gd, unsigned long long deltaTime) = 0;


		virtual void SetOnClicked(std::function<void(ITrigger*)> cb)
		{
			//save the button's feature
			this->callback = cb;

			if (this->trigger && this->callback)
			{
				//use SetOnHeldLeft just like IDraggable
				this->trigger->SetOnHeldLeft(this->callback);
			}
		}

		//check if the cursor is hovering over the button.
		virtual void Update(unsigned long long deltaTime)
		{
			if (!this->trigger)
				return;

			//update trigger
			this->trigger->Update(deltaTime);

			//change button state
			if (this->trigger->IsHeldLeft(deltaTime)) 
				this->currentState = ButtonState::CLICKED;
			else if (this->trigger->IsHovering(deltaTime)) 
				this->currentState = ButtonState::HOVER;
			else 
				this->currentState = ButtonState::IDLE;
		}

		std::shared_ptr<DX9GF::RectangleTrigger> GetTrigger() { return this->trigger; }
		float GetWidth() { return this->width; }
		float GetHeight() { return this->height; }
	};
}

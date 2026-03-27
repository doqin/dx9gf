#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <memory>
#include <functional>
namespace Demo
{
	class IButton : public DX9GF::IGameObject
	{
	protected:
		//button state to change its appearance,...
		enum class ButtonState { IDLE, HOVER, CLICKED, DISABLED };
		ButtonState currentState;
		float width;
		float height;
		std::shared_ptr<DX9GF::RectangleTrigger> trigger;

		std::function<void(DX9GF::ITrigger*)> callback;
		DX9GF::Camera* uiCamera = nullptr;
	public:

		IButton(std::shared_ptr<DX9GF::TransformManager> tm, float x, float y, float w, float h);

		virtual void Init(DX9GF::Camera* uiCamera) = 0;
		//abstract function to determine whether the button displays text or an icon
		virtual void Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime) = 0;


		virtual void SetOnClicked(std::function<void(DX9GF::ITrigger*)> cb);

		//check if the cursor is hovering over the button.
		virtual void Update(unsigned long long deltaTime);

		std::shared_ptr<DX9GF::RectangleTrigger> GetTrigger();
		float GetWidth() const;
		float GetHeight() const;
	};
}

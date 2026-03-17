#pragma once
#include "DX9GFICommand.h"
#include "../DX9GFCamera.h"

namespace DX9GF {
	class SetCameraPositionCommand : public ICommand {
	private:
		DX9GF::Camera* camera;
		float targetX;
		float targetY;
	public:
		SetCameraPositionCommand(DX9GF::Camera* camera, float x, float y);
		void Execute(unsigned long long deltaTime) override;
	};
}

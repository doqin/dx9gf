#pragma once
#include "DX9GFICommand.h"
#include "../DX9GFCamera.h"

namespace DX9GF {
	class ZoomCameraCommand : public ICommand {
	private:
		DX9GF::Camera* camera;
		float targetZoom;
		float zoomVelocity;
	public:
		ZoomCameraCommand(DX9GF::Camera* camera, float scale, float zoomVelocity);
		void Execute(unsigned long long deltaTime) override;
	};
}
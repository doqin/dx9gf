#include "DX9GFSetCameraPositionCommand.h"

namespace DX9GF {
	SetCameraPositionCommand::SetCameraPositionCommand(DX9GF::Camera* camera, float x, float y)
		: camera(camera), targetX(x), targetY(y) {
	}

	void SetCameraPositionCommand::Execute(unsigned long long deltaTime) {
		if (!camera) {
			MarkFinished();
			return;
		}

		auto pos = camera->GetPosition();
		pos.x = targetX;
		pos.y = targetY;
		camera->SetPosition(pos);

		MarkFinished();
	}
}
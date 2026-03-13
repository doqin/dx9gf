#include "DX9GFZoomCameraCommand.h"
#include <cmath>

namespace DX9GF {
	ZoomCameraCommand::ZoomCameraCommand(DX9GF::Camera* camera, float scale, float zoomVelocity)
		: camera(camera), targetZoom(scale), zoomVelocity(zoomVelocity) {
	}

	void ZoomCameraCommand::Execute(unsigned long long deltaTime) {
		if (!camera) {
			MarkFinished();
			return;
		}

		float currentZoom = camera->GetZoom();
		float diff = targetZoom - currentZoom;
		float step = zoomVelocity * (deltaTime / 1000.0f);

		if (std::abs(diff) <= step) {
			camera->SetZoom(targetZoom);
			MarkFinished();
		}
		else {
			float direction = (diff > 0) ? 1.0f : -1.0f;
			camera->SetZoom(currentZoom + direction * step);
		}
	}
}
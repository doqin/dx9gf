#include "DX9GFRotateCommand.h"
#include <cmath>

namespace DX9GF {
	RotateCommand::RotateCommand(std::weak_ptr<IGameObject> gameObject, float targetRotation, float angularVelocity)
		: targetObject(gameObject), targetRotation(targetRotation), angularVelocity(angularVelocity) {
	}

	void RotateCommand::Execute(unsigned long long deltaTime) {
		auto obj = targetObject.lock();
		if (!obj) {
			MarkFinished();
			return;
		}

		float currentRot = obj->GetLocalRotation();
		float diff = targetRotation - currentRot;
		float rotateStep = angularVelocity * (deltaTime / 1000.0f);

		if (std::abs(diff) <= rotateStep) {
			obj->SetLocalRotation(targetRotation);
			MarkFinished();
		}
		else {
			float direction = (diff > 0) ? 1.0f : -1.0f;
			obj->SetLocalRotation(currentRot + direction * rotateStep);
		}
	}
}
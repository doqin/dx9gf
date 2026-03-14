#include "pch.h"
#include "DX9GFGoToCommand.h"
#include <cmath>

namespace DX9GF {
	GoToCommand::GoToCommand(std::weak_ptr<IGameObject> gameObject, float x, float y, float velocity)
		: targetObject(gameObject), targetX(x), targetY(y), velocity(velocity) {
	}

	void GoToCommand::Execute(unsigned long long deltaTime) {
		auto obj = targetObject.lock();
		// N?u object dã b? xóa gi?a ch?ng, k?t thúc l?nh luôn d? tránh k?t Cutscene
		if (!obj) {
			MarkFinished();
			return;
		}

		float currentX = obj->GetWorldX();
		float currentY = obj->GetWorldY();

		float dx = targetX - currentX;
		float dy = targetY - currentY;
		float distance = std::sqrt(dx * dx + dy * dy);
		float moveStep = velocity * (deltaTime / 1000.0f);

		if (distance <= moveStep) {
			obj->SetLocalPosition(targetX, targetY);
			MarkFinished();
		}
		else {
			float nx = dx / distance;
			float ny = dy / distance;
			obj->SetLocalPosition(currentX + nx * moveStep, currentY + ny * moveStep);
		}
	}
}

#include "pch.h"
#include "EnergyCard.h"
#include "IBattleScene.h"

bool Demo::EnergyCard::Execute()
{
	if (isDone) {
		return true;
	}
	if (battleScene) {
		battleScene->QueueBonusEnergy(1);
		battleScene->QueuePopUpMessage(L"+1 energy next turn");
	}
	isDone = true;
	return true;
}

void Demo::EnergyCard::ResetExecution()
{
	isDone = false;
}


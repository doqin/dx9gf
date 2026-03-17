#include "pch.h"
#include "DX9GFPlaySoundCommand.h"

DX9GF::PlaySoundCommand::PlaySoundCommand(const std::string& soundId) : soundId(soundId)
{
}

void DX9GF::PlaySoundCommand::Execute(unsigned long long deltaTime)
{
	AudioManager::GetInstance()->Play(soundId);

	MarkFinished();
}
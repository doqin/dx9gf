#pragma once
#include "DX9GFICommand.h"
#include "../DX9GFAudioManager.h"
#include <string>

namespace DX9GF {
	class PlaySoundCommand : public ICommand {
	private:
		std::string soundId;
	public:
		PlaySoundCommand(const std::string& soundId);
		void Execute(unsigned long long deltaTime) override;
	};
}
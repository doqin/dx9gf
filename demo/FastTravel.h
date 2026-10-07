#pragma once
#include <string>
#include <typeindex>
#include <vector>

namespace Demo {
	// Fast travel: beating an area's boss unlocks a jump back to that area's starting point,
	// picked from the button next to the minimap. Every destination is one row in
	// Destinations() (FastTravel.cpp) - adding a map later only means adding a row there.
	namespace FastTravel {
		struct Destination {
			std::string sceneId;  // the world scene's GetSaveID(); also the key saved in PlayerGlobalData
			std::wstring name;     // English label, shown through Tr()
			float startX, startY;  // the map's starting point (the position its OnInit passes to InitCore)
			const char* bgm;       // music on arrival, as the portals into that map play
			float bgmVolume;
			std::type_index boss;	// defeating this enemy type unlocks the destination
		};

		const std::vector<Destination>& Destinations();

		// Unlocks the destination guarded by this enemy type. Returns it when this call unlocked it,
		// nullptr when no destination matches or it was already unlocked.
		const Destination* UnlockByBoss(std::type_index bossType);
	}
}
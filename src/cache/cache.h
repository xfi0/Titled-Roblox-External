#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include "sdk/sdk.h"

namespace cache {

	struct entity_t final {
		std::string name;
		std::uint8_t rigType;
		Roblox::humanoid_t humanoid;

		std::unordered_map<std::string, Roblox::part_t> parts;
	};

	inline cache::entity_t cachedLocalPlayer;
	inline std::vector<cache::entity_t> cachedPlayers;

	void Run();
}
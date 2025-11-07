#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include "sdk/sdk.h"
#include "sdk/math/math.h"
namespace cache {

	struct entity_t final {
		std::string name;
		std::uint8_t rigType;
		Roblox::humanoid_t humanoid;
		math::vector3 position;
		math::vector3 headPosition;
		math::vector3 footPosition;
		math::vector3 leftArmPosition;
		math::vector3 rightArmPosition;
		bool isLocal(const entity_t& entity); // wanted to just do a {return humanoid address...} but stupid cpp
		std::unordered_map<std::string, Roblox::part_t> parts;
	};
	inline cache::entity_t cachedLocalPlayer;
	inline std::vector<cache::entity_t> cachedPlayers;

	void Run();
}
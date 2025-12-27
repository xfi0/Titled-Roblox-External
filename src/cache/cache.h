#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include "sdk/sdk.h"
#include "sdk/math/math.h"
#include <mutex>

namespace cache {

	struct entity_t final {
		std::string name;
		std::uint8_t rigType;
		Roblox::humanoid_t humanoid;
		Roblox::instance_t head;
		Roblox::instance_t leftArm;
		Roblox::instance_t rightArm;
		Roblox::instance_t leftLeg;
		Roblox::instance_t rightLeg;
		math::vector3 position;
		math::vector3 rotation;
		float health;
		float maxHealth;
		std::uint64_t playerAddress;
		bool isLocal(const entity_t& entity);
		std::unordered_map<std::string, Roblox::part_t> parts;
	};
	inline cache::entity_t cachedLocalPlayer;
	inline std::vector<cache::entity_t> cachedPlayers;
	inline std::mutex cachedPlayersMutex;

	void Run();
}
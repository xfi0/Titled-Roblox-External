#include "cache.h"
#include <thread>
#include "game/game.h"

void cache::Run() {
	while (true) {
		std::vector<Roblox::player_t> players = game::players.GetChildren<Roblox::player_t>();

		std::vector<cache::entity_t> tempCache;
		for (Roblox::player_t& player : players) {
			cache::entity_t entity{};

			entity.name = player.GetName();
			//entity.rigTsype = 

			Roblox::model_instance_t modelInstance = player.GetModelInstance();

			auto parts = modelInstance.GetChildren<Roblox::part_t>();
			for (auto& part : parts) {
				std::string partClass = part.GetClassNameA();
				if (partClass.find("part") != std::string::npos) {
					entity.parts[part.GetName()] = part;
				}
			}

			entity.humanoid = modelInstance.FindFirstChild("Humanoid").address;
			entity.rigType = entity.humanoid.GetRigType();

			tempCache.push_back(entity);
			if (entity.name == "nonflirtatiousness") {
				cachedLocalPlayer = entity;
			}// TEST
			cachedPlayers = tempCache;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
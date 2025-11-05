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
			entity.humanoid.Health = memory->read<float>(entity.humanoid.address + Offsets::Humanoid::Health);
			entity.humanoid.MaxHealth = memory->read<float>(entity.humanoid.address + Offsets::Humanoid::MaxHealth);

			tempCache.push_back(entity);

			cachedLocalPlayer = memory->read<entity_t>(game::players.address + Offsets::Player::LocalPlayer); // might work
			cachedPlayers = tempCache;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
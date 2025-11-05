#include "cache.h"
#include <thread>
#include "game/game.h"

void cache::Run() {
	while (true) {
		try {
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
				printf("\n%s ", game::players.GetClassNameA().c_str());
				Roblox::instance_t inst = game::players.FindFirstChildByClass("Player");
				cachedLocalPlayer = {}; // clear
				if (inst.address) {
					for (auto& ent : tempCache) {
						if (ent.humanoid.address && ent.humanoid.address == inst.address) {
							cachedLocalPlayer = ent;
							break;
						}
						if (!ent.name.empty() && ent.name == inst.GetName()) { // fallback
							cachedLocalPlayer = ent;
							break;
						}
					}
				}

				cachedPlayers = tempCache;
			}
		}
		catch (...) {

		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
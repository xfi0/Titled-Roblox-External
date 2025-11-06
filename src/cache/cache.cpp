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
				auto rootPart = modelInstance.FindFirstChild("HumanoidRootPart");
				auto headPart = modelInstance.FindFirstChild("Head");
				Roblox::instance_t leftFootPart;
				if (entity.rigType == 1) // R15
					 leftFootPart = modelInstance.FindFirstChild("LeftFoot");
				else { // R6
					 leftFootPart = modelInstance.FindFirstChild("LeftFoot");
				}
				auto primitive = memory->read<uintptr_t>(rootPart.address + Offsets::BasePart::Primitive);
				auto headPrimitive = memory->read<uintptr_t>(headPart.address + Offsets::BasePart::Primitive);
				auto leftFootPrimitive = leftFootPart.address ? memory->read<uintptr_t>(leftFootPart.address + Offsets::BasePart::Primitive) : 0;

				entity.position = memory->read<math::vector3>(primitive + Offsets::BasePart::Position);
				entity.headPosition = memory->read<math::vector3>(headPrimitive + Offsets::BasePart::Position);
					entity.footPosition = memory->read<math::vector3>(leftFootPrimitive + Offsets::BasePart::Position); // breaks on r6?


				tempCache.push_back(entity);
				//printf("\n%s ", game::players.GetClassNameA().c_str());
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
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
}
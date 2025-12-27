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

				Roblox::model_instance_t modelInstance = player.GetModelInstance();
				if (!modelInstance.address)
					continue;

				auto children = modelInstance.GetChildren<Roblox::instance_t>();
				for (auto& inst : children) {
					try {
						if (!inst.address) continue;
						std::string className = inst.GetClassNameA();
						if (className.empty()) continue;
						if (className.ends_with("Part")) {
							Roblox::part_t part;
							part.address = inst.address;
							//part. = inst.GetName();
							entity.parts[inst.GetName()] = part;
						}
					}
					catch (...) {
						printf("%s", "Error In The Part Loop, Open A Issue!");
					}
				}

				auto humanoidInst = modelInstance.FindFirstChild("Humanoid");
				if (!humanoidInst.address) continue;

				entity.humanoid = humanoidInst.address;
				if (!entity.humanoid.address) continue;

				entity.rigType = entity.humanoid.GetRigType();

				entity.humanoid.Health = memory->read<float>(entity.humanoid.address + Offsets::Humanoid::Health);
				entity.humanoid.MaxHealth = memory->read<float>(entity.humanoid.address + Offsets::Humanoid::MaxHealth);
				
				if (entity.humanoid.Health <= 0.0f || entity.humanoid.MaxHealth <= 0.0f) continue;

			
				auto rootPart = modelInstance.FindFirstChild("HumanoidRootPart");
				if (!rootPart.address) continue;
				auto primitive = memory->read<uintptr_t>(rootPart.address + Offsets::BasePart::Primitive);
				if (!primitive) continue;

				entity.position = memory->read<math::vector3>(primitive + Offsets::BasePart::Position);
		
				entity.health = entity.humanoid.Health;
				entity.maxHealth = entity.humanoid.MaxHealth;

				tempCache.push_back(entity);
			}

			{
				std::lock_guard<std::mutex> lk(cachedPlayersMutex);

				cachedLocalPlayer = {};
				Roblox::instance_t inst = game::players.FindFirstChildByClass("Player");
				if (inst.address) {
					for (auto& ent : tempCache) {
						if (ent.humanoid.address && ent.humanoid.address == inst.address) {
							cachedLocalPlayer = ent;
							break;
						}
						if (!ent.name.empty() && ent.name == inst.GetName()) {
							cachedLocalPlayer = ent;
							break;
						}
					}
				}

				cachedPlayers = std::move(tempCache);
			}
		}
		catch (...) {

		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


bool cache::entity_t::isLocal(const entity_t& entity)
{
	return entity.humanoid.address == cachedLocalPlayer.humanoid.address;
}
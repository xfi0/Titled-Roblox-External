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

				auto parts = modelInstance.GetChildren<Roblox::part_t>();
				for (auto& part : parts) {
					std::string partClass = part.GetClassNameA();
					if (partClass.find("part") != std::string::npos && part.address) {
						entity.parts[part.GetName()] = part;
					}
				}

				auto humanoidInst = modelInstance.FindFirstChild("Humanoid");
				if (!humanoidInst.address) continue;
				entity.humanoid = humanoidInst.address;

				entity.rigType = entity.humanoid.GetRigType();

				entity.humanoid.Health = memory->read<float>(entity.humanoid.address + Offsets::Humanoid::Health);
				entity.humanoid.MaxHealth = memory->read<float>(entity.humanoid.address + Offsets::Humanoid::MaxHealth);
				
				if (entity.humanoid.Health <= 0.0f || entity.humanoid.MaxHealth <= 0.0f)
					continue;

				auto rootPart = modelInstance.FindFirstChild("HumanoidRootPart");
				if (!rootPart.address)
					continue;

				auto headPart = modelInstance.FindFirstChild("Head");
				if (headPart.address)
					entity.head = headPart.address;

				Roblox::instance_t leftFootPart;
				if (entity.rigType == 1) { // r15
					leftFootPart = modelInstance.FindFirstChild("LeftFoot");
					if (leftFootPart.address)
						entity.leftLeg = leftFootPart.address;
				}
				else { // r6
					leftFootPart = modelInstance.FindFirstChild("LeftLeg");
					if (leftFootPart.address)
					entity.leftLeg = leftFootPart.address;
				}

				auto leftArmPart = modelInstance.FindFirstChild("LeftArm");
				if (leftArmPart.address) 
					entity.leftArm = leftArmPart.address;

				auto rightArmPart = modelInstance.FindFirstChild("RightArm");
				if (rightArmPart.address)
					entity.rightArm = rightArmPart.address;

				auto primitive = memory->read<uintptr_t>(rootPart.address + Offsets::BasePart::Primitive);
				if (!primitive) continue;

				auto headPrimitive = headPart.address ? memory->read<uintptr_t>(headPart.address + Offsets::BasePart::Primitive) : 0;
				auto leftFootPrimitive = leftFootPart.address ? memory->read<uintptr_t>(leftFootPart.address + Offsets::BasePart::Primitive) : 0;
				auto leftArmPrimitive = leftArmPart.address ? memory->read<uintptr_t>(leftArmPart.address + Offsets::BasePart::Primitive) : 0;
				auto rightArmPrimitive = rightArmPart.address ? memory->read<uintptr_t>(rightArmPart.address + Offsets::BasePart::Primitive) : 0;

				entity.position = memory->read<math::vector3>(primitive + Offsets::BasePart::Position);
				if (headPrimitive) 
					entity.headPosition = memory->read<math::vector3>(headPrimitive + Offsets::BasePart::Position);
				if (leftFootPrimitive) 
					entity.footPosition = memory->read<math::vector3>(leftFootPrimitive + Offsets::BasePart::Position);
				if (rightArmPrimitive) 
					entity.rightArmPosition = memory->read<math::vector3>(rightArmPrimitive + Offsets::BasePart::Position);
				if (leftArmPrimitive) 
					entity.leftArmPosition = memory->read<math::vector3>(leftArmPrimitive + Offsets::BasePart::Position);

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
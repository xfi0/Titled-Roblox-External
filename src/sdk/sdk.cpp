#include "sdk.h"
#include <string>
#include <vector>
#include <iostream>
#include "memory/memory.h"
#include "offsets.h"
#include "sdk/math/math.h"
#include "modules/visuals/ESP.h"

/*

#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace Roblox {
	struct addressable_t {
		std::uint64_t address;

		addressable_t(std::uint64_t address) : address(0) {}
		addressable_t(std::uint64_t address) : address(address) {}
	};

	struct nameable_t : public addressable_t {
		using addressable_t::addressable_t;
		std::string GetName();
		std::string GetClassName();
	};

	struct  interface_t
	{
		std::vector<Roblox::instance_t> GetChildren();

		Roblox::instance_t FindFirstChild();
		Roblox::instance_t FindFirstChildByClass();
	};

	struct instance_t : public addressable_t, public interface_t {
		using addressable_t::addressable_t;
	};
}

*/

std::string Roblox::nameable_t::GetName() {
	std::uint64_t name_ptr = memory->read<std::uint64_t>(this->address + Offsets::Instance::Name);
	if (name_ptr) {
		return memory->read_string(name_ptr);
	}

	return "Unkown Name";
}

std::string Roblox::nameable_t::GetClassNameA() {
	std::uint64_t classDescriptor = memory->read<std::uint64_t>(this->address + Offsets::Instance::ClassDescriptor);
	std::uint64_t className = memory->read<std::uint64_t>(classDescriptor + Offsets::Instance::ClassName);

	if (className) {
		return memory->read_string(className);
	}

	return "Unkown Class Name";
}

std::vector<Roblox::instance_t> Roblox::interface_t::GetChildren()
{
	Roblox::instance_t* base = static_cast<Roblox::instance_t*>(this);

	std::uint64_t start{ memory->read<std::uint64_t>(base->address + Offsets::Instance::ChildrenStart) };
	std::uint64_t end { memory->read<std::uint64_t>(start + Offsets::Instance::ChildrenEnd)};

	std::vector<Roblox::instance_t> children;
	children.reserve(32);

	for (std::uint64_t instance = memory->read<std::uint64_t>(start); instance != end; instance += sizeof(std::shared_ptr<void*>)) {
		children.emplace_back(memory->read<std::uint64_t>(instance));
	}
	return children;
}

Roblox::instance_t Roblox::interface_t::FindFirstChild(std::string_view str)
{
	std::vector<Roblox::instance_t> children = this->GetChildren();

	for (Roblox::instance_t& child : children) {
		if (child.GetName() == str) {
			return child;
		}
	}
	return {};

}

Roblox::instance_t Roblox::interface_t::FindFirstChildByClass(std::string_view str)
{
	std::vector<Roblox::instance_t> children = this->GetChildren();
	
	for (Roblox::instance_t& child : children) {
		if (child.GetClassNameA() == str) {
			return child;
		}
	}
	return {};
}

Roblox::model_instance_t Roblox::player_t::GetModelInstance()
{
	return memory->read<std::uint64_t>(this->address + Offsets::Player::ModelInstance);
}


Roblox::primitive_t Roblox::part_t::GetPrimitive()
{
	return memory->read<std::uint64_t>(this->address + Offsets::BasePart::Primitive);
}

std::uint8_t Roblox::humanoid_t::GetRigType()
{
	return memory->read<std::uint64_t>(this->address + Offsets::Humanoid::RigType);
}

void Roblox::humanoid_t::SetJumpPower(float power)
{
	power = std::round(power);
	if (memory->read<float>(this->address + Offsets::Humanoid::JumpPower) != power) {
		memory->write(this->address + Offsets::Humanoid::JumpPower, power);
	}
}
void Roblox::humanoid_t::SetWalkSpeed(float power)
{
	power = std::round(power);
	if (memory->read<float>(this->address + Offsets::Humanoid::Walkspeed) != power) {
		memory->write(this->address + Offsets::Humanoid::Walkspeed, power);
		memory->write(this->address + Offsets::Humanoid::WalkspeedCheck, power);
	}
}

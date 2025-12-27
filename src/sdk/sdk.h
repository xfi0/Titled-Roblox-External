#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "memory/memory.h"
#include "offsets.h"
#include "sdk/math/math.h"

namespace Roblox {
	enum RigType : std::uint8_t {
		R15 = 1,
		R6 = 0
	};
	class instance_t;
	class primitive_t;
	class model_instance_t;

	struct addressable_t {
		std::uint64_t address;

		addressable_t() : address(0) {}
		addressable_t(std::uint64_t address) : address(address) {}
	};

	struct nameable_t : public addressable_t {
		using addressable_t::addressable_t;
		std::string name;
		std::string GetName();
		std::string GetClassNameA();
	};
	
	struct  interface_t
	{
		template <typename T>

		std::vector<T> GetChildren();

		std::vector<Roblox::instance_t> GetChildren();
		Roblox::instance_t FindFirstChild(std::string_view str);
		Roblox::instance_t FindFirstChildByClass(std::string_view str);
	};
	struct instance_t : public nameable_t, public interface_t {
		using nameable_t::nameable_t;
	};
	struct player_t final : public instance_t {
		using instance_t::instance_t;

		Roblox::model_instance_t GetModelInstance();
	};

	struct humanoid_t final : public addressable_t {
		using addressable_t::addressable_t;
		
		std::uint8_t GetRigType();
		void SetJumpPower(float power);
		void SetWalkSpeed(float power);
		float Health;
		float MaxHealth;
	};

	struct model_instance_t final : public instance_t {
		using instance_t::instance_t;
	};

	struct part_t  : public instance_t{
		using instance_t::instance_t;

		Roblox::primitive_t GetPrimitive() const;
	};

	struct primitive_t final : addressable_t  {
		using addressable_t::addressable_t;
		math::vector3 GetPosition();
	};
}

template <typename T>

std::vector<T> Roblox::interface_t::GetChildren()
{
	Roblox::instance_t* base = static_cast<Roblox::instance_t*>(this);

	std::uint64_t start = memory->read<std::uint64_t>(base->address + Offsets::Instance::ChildrenStart);
	std::uint64_t end = memory->read<std::uint64_t>(start + Offsets::Instance::ChildrenEnd);

	std::vector<T> children;
	children.reserve(32);

	for (std::uint64_t instance = memory->read<std::uint64_t>(start); instance != end; instance += sizeof(std::shared_ptr<void*>)) {
		children.emplace_back(memory->read<std::uint64_t>(instance));
	}
	return children;
}

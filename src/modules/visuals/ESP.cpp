#include "ESP.h"
#include "sdk/sdk.h"
#include "game/game.h"
#include "cache/cache.h"
#include "imgui/imgui.h"
#include "sdk/math/math.h"
#include "sdk/offsets.h"
#include <cstdint>
#include <array>
#include <string>

using BoxPts = std::tuple<math::vector2, math::vector2, math::vector2, math::vector2, math::vector2>;

math::Matrix4x4 Visuals::Visuals::GetViewMatrix()
{
	math::Matrix4x4 viewMatrix{};

	uintptr_t visualEngine = memory->read<uintptr_t>(memory->get_module_address() + Offsets::VisualEngine::Pointer);
	if (!visualEngine) {
		return viewMatrix;
	}

	viewMatrix = memory->read<math::Matrix4x4>(visualEngine + Offsets::VisualEngine::ViewMatrix);
	return viewMatrix;
}
static bool GetBoxPoints(const cache::entity_t& entity, const cache::entity_t& localPlayer, const math::Matrix4x4& view, BoxPts& outPts) {
	if (entity.humanoid.address == localPlayer.humanoid.address || !entity.humanoid.address)
		return false;

	math::vector2 footPos2D{};
	math::vector2 headPos2D{};
	std::string partToFind;
	if (entity.rigType == Roblox::R15)
		partToFind = "LeftFoot";
	else
		partToFind = "LeftLeg";

	if (math::math::WorldToScreen(entity.parts.find(partToFind)->second.GetPrimitive().GetPosition(), footPos2D, view) && math::math::WorldToScreen(entity.parts.find("Head")->second.GetPrimitive().GetPosition(), headPos2D, view)) {
		return false;

		float height = footPos2D.y - headPos2D.y;
		float width = height * 0.5f;
		float halfW = width * 0.5f;
		float centerX = footPos2D.x;

		math::vector2 topLeft{ centerX - halfW, headPos2D.y };
		math::vector2 topRight{ centerX + halfW, headPos2D.y };
		math::vector2 bottomLeft{ centerX - halfW, footPos2D.y };
		math::vector2 bottomRight{ centerX + halfW, footPos2D.y };
		math::vector2 bottomMiddle{ centerX, footPos2D.y };

		outPts = BoxPts{ topLeft, topRight, bottomLeft, bottomRight, bottomMiddle };
		return true;
	}
}
static void RenderNameESP(const cache::entity_t& entity, const cache::entity_t& localPlayer, const math::Matrix4x4& view) {
	if (entity.humanoid.address != localPlayer.humanoid.address && entity.humanoid.address) {
		math::vector2 headposition2D;

		if (math::math::WorldToScreen(entity.parts.find("Head")->second.GetPrimitive().GetPosition(), headposition2D, view) && entity.humanoid.address) {
			BoxPts rect;
			if (!GetBoxPoints(entity, localPlayer, view, rect))
				return;

			math::vector2 topLeft, topRight, bottomLeft, bottomRight, bottomMiddle;
			std::tie(topLeft, topRight, bottomLeft, bottomRight, bottomMiddle) = rect;
			
			ImGui::GetForegroundDrawList()->AddText(ImVec2(topRight.x + 5.0f, headposition2D.y), IM_COL32(255, 255, 255, 220), entity.name.c_str());
		}
	}
}
static void RenderBoxESP(const cache::entity_t& entity, const cache::entity_t& localPlayer, const math::Matrix4x4& view) {
	if (entity.humanoid.address != localPlayer.humanoid.address && entity.humanoid.address) {

		math::vector2 footPosition2D;
		math::vector2 headPosition2D;

		std::string partToFind = (entity.rigType == Roblox::R15) ? "LeftFoot" : "LeftLeg";

		auto itFoot = entity.parts.find(partToFind);
		auto itHead = entity.parts.find("Head");
		if (itFoot == entity.parts.end() || itHead == entity.parts.end())
			return;
	
		if (math::math::WorldToScreen(entity.parts.find(partToFind)->second.GetPrimitive().GetPosition(), footPosition2D, view) && math::math::WorldToScreen(entity.parts.find("Head")->second.GetPrimitive().GetPosition(), headPosition2D, view)) {
			float width = fabsf(headPosition2D.x - footPosition2D.x) > fabsf(headPosition2D.y - footPosition2D.y) ? fabsf(headPosition2D.x - footPosition2D.x) : fabsf(headPosition2D.y - footPosition2D.y) * 0.5f;
			if (!Visuals::BoxESP::filled) {
				ImGui::GetForegroundDrawList()->AddRect(ImVec2(footPosition2D.x - width / 2.f, headPosition2D.y), ImVec2(footPosition2D.x + width / 2.f, footPosition2D.y), ImGui::ColorConvertFloat4ToU32(ImVec4(Visuals::BoxESP::boxColor.x, Visuals::BoxESP::boxColor.y, Visuals::BoxESP::boxColor.z, Visuals::BoxESP::boxColor.w)), Visuals::BoxESP::rounding, 0, 2.f);
			}
			else {
				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(footPosition2D.x - width / 2.f, headPosition2D.y), ImVec2(footPosition2D.x + width / 2.f, footPosition2D.y), ImGui::ColorConvertFloat4ToU32(ImVec4(Visuals::BoxESP::boxColor.x, Visuals::BoxESP::boxColor.y, Visuals::BoxESP::boxColor.z, Visuals::BoxESP::boxColor.w)), Visuals::BoxESP::rounding, 0);
			}
		}
	}
}



static void RenderHealthBar(const cache::entity_t& entity, const cache::entity_t& localPlayer, const math::Matrix4x4& view) {
	if (entity.humanoid.address != localPlayer.humanoid.address && entity.humanoid.address) {
		math::vector2 footPosition2D;
		math::vector2 headPosition2D;
		std::string partToFind;

		if (entity.rigType == Roblox::R15)
			partToFind = "LeftFoot";
		else
			partToFind = "LeftLeg";

			if (math::math::WorldToScreen(entity.parts.find(partToFind)->second.GetPrimitive().GetPosition(), footPosition2D, view) && math::math::WorldToScreen(entity.parts.find("Head")->second.GetPrimitive().GetPosition(), headPosition2D, view)) {
			BoxPts rect;
			if (!GetBoxPoints(entity, localPlayer, view, rect))
				return;

			math::vector2 topLeft, topRight, bottomLeft, bottomRight, bottomMiddle;
			std::tie(topLeft, topRight, bottomLeft, bottomRight, bottomMiddle) = rect;

			float height = bottomLeft.y - topLeft.y;
			float width = height * 0.05f;
			float healthPercent = entity.health / entity.maxHealth;

			float barRightX = topLeft.x - 2.0f;
			float barLeftX = barRightX - width;

			ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(barLeftX - 2.0f, topLeft.y - 1.0f), ImVec2(barRightX + 0.0f, bottomLeft.y + 1.0f), IM_COL32(0, 0, 0, 150)); // health thats missing
			ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(barLeftX, bottomLeft.y - height * healthPercent), ImVec2(barRightX, bottomLeft.y), IM_COL32(0, 255, 0, 220)); // health
		}
	}
}
static const std::array<std::pair<std::string, std::string>, 14> R15boneConnections = {
std::make_pair("Head", "HumanoidRootPart"),
std::make_pair("HumanoidRootPart", "RightUpperArm"),
std::make_pair("RightUpperArm", "RightLowerArm"),
std::make_pair("RightLowerArm", "RightHand"),
std::make_pair("HumanoidRootPart", "LeftUpperArm"),
std::make_pair("LeftUpperArm", "LeftLowerArm"),
std::make_pair("LeftLowerArm", "LeftHand"),
std::make_pair("HumanoidRootPart", "LowerTorso"),
std::make_pair("LowerTorso", "LeftUpperLeg"),
std::make_pair("LeftUpperLeg", "LeftLowerLeg"),
std::make_pair("LeftLowerLeg", "LeftFoot"),
std::make_pair("LowerTorso", "RightUpperLeg"),
std::make_pair("RightUpperLeg", "RightLowerLeg"),
std::make_pair("RightLowerLeg", "RightFoot"),

};
static const std::array<std::pair<std::string, std::string>, 7> R6boneConnections = {
std::make_pair("Head", "Torso"),
std::make_pair("Torso", "LeftArm"),
std::make_pair("Torso", "RightArm"),
std::make_pair("Torso", "LeftLeg"),
std::make_pair("Torso", "RightLeg"),
};
static void RenderBoneESP(const cache::entity_t& entity, const cache::entity_t& localPlayer, const math::Matrix4x4& view) {
	if (!entity.humanoid.address || entity.humanoid.address == localPlayer.humanoid.address || entity.health <= 0 || entity.parts.size() <= 0)
		return;
	
	auto humanoid = entity.humanoid;
	if (humanoid.GetRigType() == Roblox::R15) {
		for (const auto& [parent, child] : R15boneConnections) {
			auto partA = entity.parts.find(parent);
			auto partB = entity.parts.find(child);
			if (partA == entity.parts.end() || partB == entity.parts.end()) continue;

			math::vector2 partAPos2D, partBPos2D;

			if (!math::math::WorldToScreen(partA->second.GetPrimitive().GetPosition(), partAPos2D, view) || !math::math::WorldToScreen(partB->second.GetPrimitive().GetPosition(), partBPos2D, view))
				continue;
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(partAPos2D.x, partAPos2D.y), ImVec2(partBPos2D.x, partBPos2D.y), ImGui::ColorConvertFloat4ToU32(ImVec4(Visuals::BoneESP::boneColor.x, Visuals::BoneESP::boneColor.y, Visuals::BoneESP::boneColor.z, Visuals::BoneESP::boneColor.w)), Visuals::BoneESP::boneThickness);
		}
	}
	else if (humanoid.GetRigType() == Roblox::R6) {
		for (const auto& [parent, child] : R6boneConnections) {
			auto partA = entity.parts.find(parent);
			auto partB = entity.parts.find(child);
			if (partA == entity.parts.end() || partB == entity.parts.end()) continue;
			math::vector2 partAPos2D, partBPos2D;

			if (!math::math::WorldToScreen(partA->second.GetPrimitive().GetPosition(), partAPos2D, view) || !math::math::WorldToScreen(partB->second.GetPrimitive().GetPosition(), partBPos2D, view))
				continue;

			ImGui::GetForegroundDrawList()->AddLine(ImVec2(partAPos2D.x, partAPos2D.y), ImVec2(partBPos2D.x, partBPos2D.y), ImGui::ColorConvertFloat4ToU32(ImVec4(Visuals::BoneESP::boneColor.x, Visuals::BoneESP::boneColor.y, Visuals::BoneESP::boneColor.z, Visuals::BoneESP::boneColor.w)), Visuals::BoneESP::boneThickness);
		}
	}
	//for (const auto& partPair : entity.parts) {
		//printf("part name: %s", partPair.first.c_str());
	//}
}

void Visuals::BoxESP::RenderLoop()
{
	math::Matrix4x4 view = Visuals::GetViewMatrix();

	std::vector<cache::entity_t> snapshot;
	cache::entity_t localPlayerSnapshot;
	{
		std::lock_guard<std::mutex> lk(cache::cachedPlayersMutex);
		snapshot = cache::cachedPlayers;
		localPlayerSnapshot = cache::cachedLocalPlayer;
	}

	if (BoxESP::boxESPEnabled) {
		for (const auto& entity : snapshot) {
			RenderBoxESP(entity, localPlayerSnapshot, view);
		}
	}
	if (NameESP::nameESPEnabled) {
		for (const auto& entity : snapshot) {
			RenderNameESP(entity, localPlayerSnapshot, view);
		}
	}
	if (HealthBar::healthBarEnabled) {
		for (const auto& entity : snapshot) {
			RenderHealthBar(entity, localPlayerSnapshot, view);
		}
	}
	if (BoneESP::boneESPEnabled) {
		for (const auto& entity : snapshot) {
			RenderBoneESP(entity, localPlayerSnapshot, view);
		}
	}
}
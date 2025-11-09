#include "ESP.h"
#include "sdk/sdk.h"
#include "game/game.h"
#include "cache/cache.h"
#include "imgui/imgui.h"
#include "sdk/math/math.h"
#include "sdk/offsets.h"
#include <cstdint>

using BoxPts = std::tuple<math::vector2, math::vector2, math::vector2, math::vector2, math::vector2>;

math::Matrix4x4 Visuals::BoxESP::GetViewMatrix()
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

	if (!math::math::WorldToScreen(entity.footPosition, footPos2D, view) || !math::math::WorldToScreen(entity.headPosition, headPos2D, view))
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
static void RenderNameESP(const cache::entity_t& entity, const cache::entity_t& localPlayer, const math::Matrix4x4& view) {
	if (entity.humanoid.address != localPlayer.humanoid.address && entity.humanoid.address) {
		math::vector2 headposition2D;

		if (math::math::WorldToScreen(entity.headPosition, headposition2D, view) && entity.humanoid.address) {
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
		if (math::math::WorldToScreen(entity.footPosition, footPosition2D, view) && math::math::WorldToScreen(entity.headPosition, headPosition2D, view)) {

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
		if (math::math::WorldToScreen(entity.footPosition, footPosition2D, view) && math::math::WorldToScreen(entity.headPosition, headPosition2D, view)) {
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

void Visuals::BoxESP::RenderLoop()
{
	math::Matrix4x4 view = GetViewMatrix();

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
}
#include "ESP.h"
#include "sdk/sdk.h"
#include "game/game.h"
#include "cache/cache.h"
#include "imgui/imgui.h"
#include "sdk/math/math.h"
#include "sdk/offsets.h"



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
static void RenderNameESP(cache::entity_t entity, math::Matrix4x4 view) {
	if (entity.humanoid.address != cache::cachedLocalPlayer.humanoid.address) {
		math::vector2 headposition2D;
		if (math::math::WorldToScreen(entity.headPosition, headposition2D, view) && entity.humanoid.address) {
			ImGui::GetBackgroundDrawList()->AddText(ImVec2(headposition2D.x, headposition2D.y), IM_COL32(255, 255, 255, 220), entity.name.c_str());
		}
	}
}
static void RenderBoxESP(cache::entity_t entity, math::Matrix4x4 view) {
	if (entity.humanoid.address != cache::cachedLocalPlayer.humanoid.address) {
		math::vector2 footPosition2D;
		math::vector2 headposition2D;
		if (math::math::WorldToScreen(entity.position, footPosition2D, view) && math::math::WorldToScreen(entity.headPosition, headposition2D, view) && entity.humanoid.address) {
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(footPosition2D.x - ((headposition2D.y - footPosition2D.y) * 0.225f), headposition2D.y), ImVec2(footPosition2D.x + ((headposition2D.y - footPosition2D.y) * 0.225f), footPosition2D.y), IM_COL32(255, 0, 0, 200));
		}
	}
}
void Visuals::BoxESP::RenderLoop()
{
	math::Matrix4x4 view = GetViewMatrix();
	if (BoxESP::boxESPEnabled) {
		for (auto& entity : cache::cachedPlayers) {
			RenderBoxESP(entity, view);
		}
	}
	if (NameESP::nameESPEnabled) {
		for (auto& entity : cache::cachedPlayers) {
			RenderNameESP(entity, view);
		}
	}
}

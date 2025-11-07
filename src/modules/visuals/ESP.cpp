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
static void RenderNameESP(const cache::entity_t& entity, const math::Matrix4x4& view) {
	if (entity.humanoid.address != cache::cachedLocalPlayer.humanoid.address && entity.humanoid.address) {
		math::vector2 headposition2D;

		if (math::math::WorldToScreen(entity.headPosition, headposition2D, view) && entity.humanoid.address) {
			ImGui::GetForegroundDrawList()->AddText(ImVec2(headposition2D.x, headposition2D.y), IM_COL32(255, 255, 255, 220), entity.name.c_str());
		}
	}
}
static void RenderBoxESP(const cache::entity_t& entity, const math::Matrix4x4& view) {
    if (entity.humanoid.address != cache::cachedLocalPlayer.humanoid.address && entity.humanoid.address) {
        math::vector2 footPosition2D;
        math::vector2 headPosition2D;
		if (math::math::WorldToScreen(entity.footPosition, footPosition2D, view) && math::math::WorldToScreen(entity.headPosition, headPosition2D, view)) {

			float width = (headPosition2D.y - footPosition2D.y) * 0.5f;
			if (!Visuals::BoxESP::filled) {
				ImGui::GetForegroundDrawList()->AddRect(ImVec2(footPosition2D.x - width / 2.f, headPosition2D.y), ImVec2(footPosition2D.x + width / 2.f, footPosition2D.y), ImGui::ColorConvertFloat4ToU32(ImVec4(Visuals::BoxESP::boxColor.x, Visuals::BoxESP::boxColor.y, Visuals::BoxESP::boxColor.z, Visuals::BoxESP::boxColor.w)), Visuals::BoxESP::rounding, 0, 2.f);
			}
			else {
				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(footPosition2D.x - width / 2.f, headPosition2D.y), ImVec2(footPosition2D.x + width / 2.f, footPosition2D.y), ImGui::ColorConvertFloat4ToU32(ImVec4(Visuals::BoxESP::boxColor.x, Visuals::BoxESP::boxColor.y, Visuals::BoxESP::boxColor.z, Visuals::BoxESP::boxColor.w)), Visuals::BoxESP::rounding, 0);
			}
		}
    }
}

void Visuals::BoxESP::RenderLoop()
{
	math::Matrix4x4 view = GetViewMatrix();
	if (BoxESP::boxESPEnabled) {
		for (const auto& entity : cache::cachedPlayers) {
			RenderBoxESP(entity, view);
		}
	}
	if (NameESP::nameESPEnabled) {
		for (const auto& entity : cache::cachedPlayers) {
			RenderNameESP(entity, view);
		}
	}
}
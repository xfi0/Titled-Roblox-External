#include "aimbot.h"
#include "sdk/sdk.h"
#include "game/game.h"
#include "cache/cache.h"
#include "imgui/imgui.h"
#include "modules/visuals/ESP.h"

void aimbot::Run()
{
    DrawFOVCircle();

    POINT curentPosition;
    GetCursorPos(&curentPosition);
    for (auto& entity : cache::cachedPlayers) {
        if (!entity.isLocal(entity) && GetAsyncKeyState(aimKey)) {
            math::vector2 target2D;
            if (math::math::WorldToScreen(entity.headPosition, target2D, Visuals::BoxESP::GetViewMatrix())) {
                float distanceToBody = math::vector2::Distance(math::vector2(curentPosition.x, curentPosition.y), target2D);
                if (!useFOV || distanceToBody < fovRadius) {
                    float dx = target2D.x - (float)curentPosition.x;
                    float dy = target2D.y - (float)curentPosition.y;
                    
                    dx *= (1.0f - smoothing);
                    dy *= (1.0f - smoothing);
                    if (!(fabsf(dx) < 0.5f && fabsf(dy) < 0.5f)) {
                        SetCursorPos((int)target2D.x, (int)target2D.y);
                        curentPosition.x = (LONG)(curentPosition.x + dx);
                        curentPosition.y = (LONG)(curentPosition.y + dy);
                    }
                }
            }
        }
    }
}


void aimbot::DrawFOVCircle()
{
	if (drawFOV) {
        POINT curentPosition;
        GetCursorPos(&curentPosition);
		ImGui::GetForegroundDrawList()->AddCircle(ImVec2(curentPosition.x, curentPosition.y), fovRadius, IM_COL32(255, 0, 0, 150), 100, 2.0f);
	}
}

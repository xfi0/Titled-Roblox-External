#include "math.h"
#include <Windows.h>
#include "modules/visuals/ESP.h"

bool math::math::WorldToScreen(const vector3& world_pos, vector2& screen_pos, const Matrix4x4& view_matrix)
{
    const float* m = &view_matrix.m[0][0];

    _mm_prefetch((const char*)m, _MM_HINT_T0);

    float w = m[12] * world_pos.x + m[13] * world_pos.y + m[14] * world_pos.z + m[15];

    if (w <= 0.001f)
        return false;

    float inv_w = 1.0f / w;

    screen_pos.x = (m[0] * world_pos.x + m[1] * world_pos.y + m[2] * world_pos.z + m[3]) * inv_w;
    screen_pos.y = (m[4] * world_pos.x + m[5] * world_pos.y + m[6] * world_pos.z + m[7]) * inv_w;

    const float screen_width_half = static_cast<float>(GetSystemMetrics(SM_CXSCREEN)) * 0.5f;
    const float screen_height_half = static_cast<float>(GetSystemMetrics(SM_CYSCREEN)) * 0.5f;

    screen_pos.x = screen_width_half * (screen_pos.x + 1.0f);
    screen_pos.y = screen_height_half * (1.0f - screen_pos.y);

    return true;
}

math::vector2 math::vector3::WorldToScreen() // TODO: fix when not visible it draws
{
    vector2 tempPos;
    if (math::WorldToScreen(*this, tempPos, Visuals::Visuals::GetViewMatrix())) {
        return tempPos;
    }
}

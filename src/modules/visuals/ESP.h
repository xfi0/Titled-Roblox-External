#pragma once
#include "sdk/math/math.h"
#include "imgui/imgui.h"
#include "sdk/math/math.h"
namespace Visuals {
    class BoxESP {
    public:
        inline static bool boxESPEnabled = true;
        static void RenderLoop();
        static math::Matrix4x4 GetViewMatrix();
        inline static float rounding = 0.3f;
        inline static bool filled = false;
        inline static math::vector4 boxColor = math::vector4(255, 0, 0, 255);
    };

    class NameESP {
    public:
        inline static bool nameESPEnabled = true;
    };
}
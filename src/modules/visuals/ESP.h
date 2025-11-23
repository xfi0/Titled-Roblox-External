#pragma once
#include "sdk/math/math.h"
#include "imgui/imgui.h"
#include "sdk/math/math.h"

namespace Visuals {
    class Visuals {
    public:
        inline static math::Matrix4x4 GetViewMatrix();
    };
    class BoxESP {
    public:
        inline static bool boxESPEnabled = true;
        static void RenderLoop();
        inline static float rounding = 0.3f;
        inline static bool filled = false;
        inline static math::vector4 boxColor = math::vector4(255, 0, 0, 255);
    };
    class BoneESP {
    public:
        inline static bool boneESPEnabled = true;
        inline static math::vector4 boneColor = math::vector4(0, 255, 0, 255);
        inline static float boneThickness = 2.0f;
    };
    class NameESP {
    public:
        inline static bool nameESPEnabled = true;
    };
    class HealthBar {
    public:
		inline static bool healthBarEnabled = true;
    };
}
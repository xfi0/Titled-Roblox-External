#pragma once
#include "Windows.h"

class aimbot {
public:
	inline static bool aimbotEnabled = false;
	inline static bool drawFOV = true;
	inline static bool useFOV = true;
	inline static float fovRadius = 100.0f;
	static void Run();
	static void DrawFOVCircle();
	inline static int aimKey = VK_RBUTTON; // 0x02
	inline static int smoothing = 6;
};
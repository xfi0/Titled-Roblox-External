#pragma once
#include "sdk/math/math.h"

class movement {
public:
	static void Teleport(math::vector3 Position);
	static void SavePosition();
	static void NoClip();
	static void SpinBot();
};
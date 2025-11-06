#pragma once
#include "sdk/math/math.h"
namespace Visuals {
	class BoxESP {
	public:
		inline static bool boxESPEnabled = true;
		static void RenderLoop();
		static math::Matrix4x4 GetViewMatrix();
	};
	class NameESP {
	public :
		inline static bool nameESPEnabled = true;
	};
}
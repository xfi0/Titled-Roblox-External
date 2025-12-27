#pragma once
#include <cmath> 

namespace math {

	struct vector2 final {
		float x{ 0.f }, y{ 0.f };

		static float Distance(const vector2& a, const vector2& b) {
			float dx = a.x - b.x;
			float dy = a.y - b.y;
			return std::sqrt(dx * dx + dy * dy);
		}
	};
	struct vector3 final {
		float x{ 0.f }, y{ 0.f }, z{ 0.f };
		vector2 WorldToScreen();
	};
	struct vector4 final {
		float x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 0.f };
	};
	struct Matrix4x4
	{
		float m[4][4];
	};
	struct Matrix4x3
	{
		float m[4][3];
	};
	class math {
	public:
		static bool WorldToScreen(const vector3& world_pos, vector2& screen_pos, const Matrix4x4& view_matrix);
	};
}
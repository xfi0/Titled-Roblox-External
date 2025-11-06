#pragma once

namespace math {

	struct vector2 final {
		float x{ 0.f }, y{ 0.f };
	};
	struct vector3 final {
		float x{ 0.f }, y{ 0.f }, z{ 0.f };
	};
	struct vector4 final {
		float x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 0.f };
	};
	struct Matrix4x4
	{
		float m[4][4];
	};
	class math {
	public:
		static bool WorldToScreen(const vector3& world_pos, vector2& screen_pos, const Matrix4x4& view_matrix);
	};
}
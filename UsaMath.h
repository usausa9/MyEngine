#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include "Matrix4.h"

#include <cmath>

namespace UsaMath
{
	const double u_PI = 3.141592653589;

	inline float DegreesToRadians(float Degrees)
	{
		return Degrees * (u_PI / 180.0f);
	}

	inline float RadiansToDegrees(float Radians)
	{
		return Radians * (180.0f / u_PI);
	}
}
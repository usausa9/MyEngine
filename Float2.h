#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Float3.h"
#include "Float4.h"

struct Float2
{
	float x;
	float y;

	void operator = (const Float2& f2)
	{
		x = f2.x; y = f2.y;
	};

	operator Vector2() const { return Vector2(x, y); };
	operator Vector3() const { return Vector3(x, y, 0); };

	operator Float3()
	{
		return { x, y, 0, };
	}

	operator Float4()
	{
		return { x, y, 0, 0 };
	}
};
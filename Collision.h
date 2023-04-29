#pragma once

#include "CollisionPrimitive.h"

using namespace DirectX;

class Collision {
public:
	static bool Col_Sphere_2_Plane
	(const Sphere& sphere, const Plane& plane);

};
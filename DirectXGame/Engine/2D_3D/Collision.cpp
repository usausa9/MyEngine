#include "Collision.h"

bool Collision::Col_Sphere_2_Plane
(const Sphere& sphere, const Plane& plane)
{
	// 原点から球の中心座標
	float distV = sphere.center.Dot(plane.normal);

	// 平面の原点距離減算 (平面と球の中心との距離)
	float dist = distV - plane.distance;

	// 距離の絶対値が半径よりも大きければ当たっていない
	if (fabsf(dist) > sphere.radius)
	{
		return false;
	}

	return true;
}

#pragma once

#include <DirectXMath.h>
#include "Vector3.h"

using namespace DirectX;

struct Sphere // 球
{
	// 中心座標
	Vector3 center = { 0,0,0 };

	// 半径
	float radius = 1.0f;
};

struct Plane // 平面
{
	// 法線ベクトル
	Vector3 normal = { 0,1,0 };

	// 原点からの距離
	float distance = 0.0f;
};
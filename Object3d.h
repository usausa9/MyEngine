#pragma once
#include "Common.h"
#include "Vector3.h"
#include "Model.h"

// 定数バッファ用データ構造体 (3D変換行列)
struct Object3DConstBufferDataTransform
{
	Matrix4 mat; // 3D変換行列
};

class Object3D
{
public:
	// 定数バッファ (行列用)
	ID3D12Resource* constBuffTransform = nullptr;

	// 定数バッファマップ (行列用)
	Object3DConstBufferDataTransform* constMapTransform = nullptr;

	// アフィン変換情報
	Vector3 scale = { 1,1,1 };
	Vector3 rotation = { 0,0,0 };
	Vector3 position = { 0,0,0 };

	// ワールド変換行列
	Matrix4 matWorld = matWorld.Identity();

	// 親オブジェクトへのポインタ
	Object3D* parent = nullptr;

	Model* model = nullptr;

public:
	// 3Dオブジェクト初期化
	void InitializeObject3D();

	// 3Dオブジェクト更新処理
	void UpdateObject3D();

	// 3Dオブジェクト描画処理
	void DrawObject3D();
};


#pragma once
#include "Common.h"

// 定数バッファ用データ構造体 (3D変換行列)
struct CameraConstBufferData
{
	Matrix4 view; 
	Matrix4 projection;
	Matrix4 billboard;
	Float3 position;
};

class Camera
{
public:
	~Camera();

public:
	// 定数バッファ (行列用)
	ID3D12Resource* constBuffCamera = nullptr;

	// 定数バッファマップ (行列用)
	CameraConstBufferData* constMapCamera = nullptr;

	// アフィン変換情報
	// Float3 rotation = { 0,0,0 };
	Float3 target = { 0,0,0 };
	Float3 position = { 0,0,-20 };

	// 3D用の射影変換
	Matrix4 matProjection = Matrix4::Identity();

	// ビュー変換行列
	Matrix4 matView = matView.Identity();
	Float3 up = { 0,1,0 };
	
	// ビルボード行列
	Matrix4 matBillboard = Matrix4::Identity();

public:
	// カメラ初期化
	void Initialize();

	// カメラ更新処理
	void Update();

	void Set();

	// ビュープロジェクション
	Matrix4 GetViewProjection()const { return matView * matProjection; }
	
	// カメラ座標取得
	Float3 GetEye()const { return position; }
};
#pragma once
#include "Common.h"

// 定数バッファ用データ構造体 (3D変換行列)
struct CameraConstBufferData
{
	XMMATRIX view; 
	XMMATRIX projection;
	XMMATRIX billboard;
	XMFLOAT3 position;
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
	// XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 target = { 0,0,0 };
	XMFLOAT3 position = { 0,0,-20 };

	// 3D用の射影変換
	XMMATRIX matProjection = XMMatrixIdentity();

	// ビュー変換行列
	XMMATRIX matView = XMMatrixIdentity();
	XMFLOAT3 up = { 0,1,0 };
	
	// ビルボード行列
	XMMATRIX matBillboard = XMMatrixIdentity(); 

public:
	// カメラ初期化
	void Initialize();

	// カメラ更新処理
	void Update();

	void Set();
};
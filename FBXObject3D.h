#pragma once

#include "Model.h"
#include "Camera.h"

#include "Common.h"

#include <Windows.h>

class FBXObject3D
{
protected:	// エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	// サブクラス
	// 定数バッファ用データ構造体 (座標変換行列用)
	struct ConstBufferDataTransform
	{
		Matrix4 viewProjection;	// ビュープロジェクション行列
		Matrix4 worldTransform;	// ワールド行列
		Float3  cameraPosition;	// カメラ座標
	};

public:	// メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="model"> モデル </param>
	void SetModel(Model* model) { this->model = model; }

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList"></param>
	void Draw();

protected:	// メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;

	// ローカルスケール
	Vector3 scale = { 1,1,1 };
	// X,Y,Z軸周りのローカル回転角
	Vector3 rotation = { 0,0,0 };
	// ローカル座標
	Vector3 position = { 0,0,0 };
	// ローカルワールド変換行列
	Matrix4 matWorld;
	// モデル
	Model* model = nullptr;

public:	// 静的メンバ関数
	// setter
	static void SetDevice(ID3D12Device* device) { FBXObject3D::device = device; }
	static void SetCamera(Camera* camera) { FBXObject3D::camera = camera; }
	static void SetCommandList(ID3D12GraphicsCommandList* commandList) { FBXObject3D::commandList = commandList; }
	static void CreateGraphicsPipeline();	// パイプラインの生成

private: // 静的メンバ変数
	
	static ID3D12GraphicsCommandList* commandList;	//コマンドリスト
	static ID3D12Device* device;	// デバイス
	static Camera* camera;			// カメラ
	static ComPtr<ID3D12RootSignature> rootSigunature;
	static ComPtr<ID3D12PipelineState> pipelineState;
};


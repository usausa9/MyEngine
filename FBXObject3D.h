#pragma once
#include "FBXModel.h"
#include "FbxLoader.h"
#include "Camera.h"
#include "Common.h"

#include <Windows.h>

class FBXObject3D
{
protected:	// エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // 定数
	// ボーンの最大数
	static const int MAX_BONES = 32;

public:	// サブクラス
	// 定数バッファ用データ構造体 (座標変換行列用)
	struct ConstBufferDataTransform
	{
		Matrix4 viewProjection;	// ビュープロジェクション行列
		Matrix4 worldTransform;	// ワールド行列
		Float3  cameraPosition;	// カメラ座標
	};

	// 定数バッファ用データ構造体 (スキニング)
	struct ConstBufferDataSkin
	{
		Matrix4 bones[MAX_BONES];
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
	void SetModel(FBXModel* model) { this->model = model; }

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList"></param>
	void Draw();

	/// <summary>
	/// アニメーション開始
	/// </summary>
	void PlayAnimation();

protected:	// メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;
	// 定数バッファ (スキン)
	ComPtr<ID3D12Resource> constBuffSkin;

	// ローカルスケール
	Vector3 scale = { 1,1,1 };
	// X,Y,Z軸周りのローカル回転角
	Vector3 rotation = { 0,0,0 };
	// ローカル座標
	Vector3 position = { 0,0,0 };
	// ローカルワールド変換行列
	Matrix4 matWorld = {};
	// モデル
	FBXModel* model = nullptr;

public:
	// 1フレームの時間
	FbxTime frameTime;
	// アニメーション開始時間
	FbxTime startTime;
	// アニメーション終了時間
	FbxTime endTime;
	// 現在時間 (アニメーション)
	FbxTime currentTime;
	// アニメーション再生中
	bool isPlay = false;

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


#pragma once
#include "Common.h"

#include "Input.h"
#include "WinAPI.h"
#include "DirectXBase.h"

#include "ParticleManager.h"
#include "SpriteManager.h"

#include "FbxLoader.h"

#include "GameScene.h"

#include <Windows.h>
#include <cmath>

#include <DirectXTex.h>

class MyGame
{
public:
	GameScene scene;

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	// メンバ関数

	// 初期化
	void Initialize();

	// 終了
	void Finalize();

	// 毎フレーム更新
	void Update();

	// 描画前処理
	void PreDraw();

	// パーティクル描画前処理
	void PreDrawParticle();

	// 描画
	void Draw();

	// 描画後処理
	void PostDraw();

	// 終了フラグチェック
	bool IsEndRequest() { return endRequest_; }

private: // メンバ変数

	unique_ptr<SpriteManager> spriteManager = nullptr;

	// 終了フラグ
	bool endRequest_ = false;

	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	// WindowsAPI初期化
	WinAPI& window = *WinAPI::Get();
};


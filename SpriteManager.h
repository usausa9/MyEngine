#pragma once
#include "Common.h"
#include "TextureManager.h"

#include "WinAPI.h"

// 定数バッファ用 データ構造体 (マテリアル)
struct SpriteConstBufferDataMaterial
{
	Float4 color; // 色(RGBA)
	XMMATRIX mat; // 3D変換行列
};

class SpriteManager
{
public:

	// 初化
	void Init();

	void PreDraw();

public: // メンバ変数

	ComPtr<ID3DBlob> vsBlob = nullptr;		 // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;		 // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;	 // エラーオブジェクト

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	// パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;

public:
	static XMMATRIX SpriteProjection;
};
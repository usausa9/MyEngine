#pragma once
#include "Sprite.h"
#include "Common.h"

class PostEffect : public Sprite
{
private: // サブクラス

	struct VertexPosUv 
	{
		Float3 pos;	// xyz座標
		Float2 uv;	// uv 座標
	};

	struct ConstBufferData 
	{
		Float4 color;
		Matrix4 mat;
	};

private: // 静的メンバ変数
	// 画面クリアカラー
	static const float clearColor[4];

public:	// メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// パイプライン生成
	/// </summary>
	void CreateGraphicsPipelineState();

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// シーン描画前処理
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDrawScene(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// シーン描画後処理
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PostDrawScene(ID3D12GraphicsCommandList* commandList);

private: // DirectX ポインタ類
	// バッファ類
	ComPtr<ID3D12Resource> texBuff = nullptr;
	ComPtr<ID3D12Resource> constBuff = nullptr;
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	ComPtr<ID3D12Resource> depthBuff = nullptr;

	// グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;

	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV = nullptr;

	// RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV = nullptr;
	
	// DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV = nullptr;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
};
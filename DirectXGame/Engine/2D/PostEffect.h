#pragma once
#include "Sprite.h"
#include "Common.h"

class PostEffect : public Sprite
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect(TextureIndex tex);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

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

private:
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff = nullptr;

	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV = nullptr;

	// 深度バッファ
	ComPtr<ID3D12Resource> depthBuff = nullptr;

	// RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV = nullptr;
	
	// DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV = nullptr;

	// 画面クリアカラー
	static const float clearColor[4];
};
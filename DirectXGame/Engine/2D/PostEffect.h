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

private:
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff = nullptr;

	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV = nullptr;
};
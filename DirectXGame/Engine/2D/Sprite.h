#pragma once
#include "Common.h"
#include "SpriteManager.h"

class Sprite
{
public:
	Sprite();
	Sprite(TextureIndex tex);
	Sprite(TextureIndex texIndex, Float2 pos, Float2 scale_, Float4 color_, Float2 anchorP, bool isFlipX_, bool isFlipY_);
	~Sprite();

public:
	// 初期化
	void Init();

	void Update();
	void Draw();

protected: // メンバ変数
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{}; // 頂点バッファビューの作成

	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;	// 定数バッファ
	SpriteConstBufferDataMaterial* constMapMaterial = nullptr;// マップの割り当て

	TextureIndex tIndex = 0;	// 描画するものを入れられるようにする
	
public:
	Float2 position = { 0,0 };
	Float2 scale = { 1,1 };
	float rotation = 0;

	Float4 color = { 1.0f,1.0f,1.0f,1.0f };	// 座標
	Float2 anchorPoint = { 0.0f,0.0f };		// アンカーポイント(座標変換の基準点)
	bool isFlipX = false;	// 左右反転フラグ
	bool isFlipY = false;	// 上下反転フラグ
};
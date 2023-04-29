#pragma once
#include "Common.h"
#include "SpriteManager.h"

class Sprite
{
public:
	Sprite();
	Sprite(TextureIndex tex);
	~Sprite();

private:
	// 初期化
	void Init();

public:
	void Update();
	void Draw();

public: // メンバ変数
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{}; // 頂点バッファビューの作成

	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;	// 定数バッファ
	SpriteConstBufferDataMaterial* constMapMaterial = nullptr;// マップの割り当て

public:
	TextureIndex tIndex = 0;	// 描画するものを入れられるようにする

	XMFLOAT2 position = { 100,100 };
	XMFLOAT2 scale = { 1,1 };
	float rotation = 0;
};
#pragma once
#include "Sprite.h"
#include "Common.h"

class PostEffect : public Sprite
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect();
	PostEffect(TextureIndex tex);

	/// <summary>
	///  描画コマンドの発行
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList);
};
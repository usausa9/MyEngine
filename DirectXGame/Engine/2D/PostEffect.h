#pragma once
#include "Sprite.h"
#include "Common.h"

class PostEffect : public Sprite
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	PostEffect();
	PostEffect(TextureIndex tex);

	/// <summary>
	///  �`��R�}���h�̔��s
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList);
};
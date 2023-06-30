#pragma once
#include "Sprite.h"
#include "Common.h"

class PostEffect : public Sprite
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	PostEffect(TextureIndex tex);

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �`��R�}���h�̔��s
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList);

private:
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff = nullptr;

	// SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV = nullptr;
};
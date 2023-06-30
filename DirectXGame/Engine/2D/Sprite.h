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
	// ������
	void Init();

	void Update();
	void Draw();

protected: // �����o�ϐ�
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{}; // ���_�o�b�t�@�r���[�̍쐬

	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;	// �萔�o�b�t�@
	SpriteConstBufferDataMaterial* constMapMaterial = nullptr;// �}�b�v�̊��蓖��

	TextureIndex tIndex = 0;	// �`�悷����̂�������悤�ɂ���
	
public:
	Float2 position = { 0,0 };
	Float2 scale = { 1,1 };
	float rotation = 0;

	Float4 color = { 1.0f,1.0f,1.0f,1.0f };	// ���W
	Float2 anchorPoint = { 0.0f,0.0f };		// �A���J�[�|�C���g(���W�ϊ��̊�_)
	bool isFlipX = false;	// ���E���]�t���O
	bool isFlipY = false;	// �㉺���]�t���O
};
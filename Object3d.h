#pragma once
#include "Common.h"
#include "Vector3.h"
#include "Model.h"

// �萔�o�b�t�@�p�f�[�^�\���� (3D�ϊ��s��)
struct object3DConstBufferDataTransform
{
	XMMATRIX mat; // 3D�ϊ��s��
};

class object3D
{
public:
	// �萔�o�b�t�@ (�s��p)
	ID3D12Resource* constBuffTransform = nullptr;

	// �萔�o�b�t�@�}�b�v (�s��p)
	object3DConstBufferDataTransform* constMapTransform = nullptr;

	// �A�t�B���ϊ����
	Vector3 scale = { 1,1,1 };
	Vector3 rotation = { 0,0,0 };
	Vector3 position = { 0,0,0 };

	// ���[���h�ϊ��s��
	XMMATRIX matWorld = XMMatrixIdentity();

	// �e�I�u�W�F�N�g�ւ̃|�C���^
	object3D* parent = nullptr;

	Model* model = nullptr;

public:
	// 3D�I�u�W�F�N�g������
	void InitializeObject3D();

	// 3D�I�u�W�F�N�g�X�V����
	void UpdateObject3D();

	// 3D�I�u�W�F�N�g�`�揈��
	void DrawObject3D();
};

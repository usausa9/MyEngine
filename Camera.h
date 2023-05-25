#pragma once
#include "Common.h"

// �萔�o�b�t�@�p�f�[�^�\���� (3D�ϊ��s��)
struct CameraConstBufferData
{
	Matrix4 view; 
	Matrix4 projection;
	Matrix4 billboard;
	Float3 position;
};

class Camera
{
public:
	~Camera();

public:
	// �萔�o�b�t�@ (�s��p)
	ID3D12Resource* constBuffCamera = nullptr;

	// �萔�o�b�t�@�}�b�v (�s��p)
	CameraConstBufferData* constMapCamera = nullptr;

	// �A�t�B���ϊ����
	// Float3 rotation = { 0,0,0 };
	Float3 target = { 0,0,0 };
	Float3 position = { 0,0,-20 };

	// 3D�p�̎ˉe�ϊ�
	Matrix4 matProjection = matProjection.Identity();

	// �r���[�ϊ��s��
	Matrix4 matView = matView.Identity();
	Float3 up = { 0,1,0 };
	
	// �r���{�[�h�s��
	Matrix4 matBillboard = matBillboard.Identity();

public:
	// �J����������
	void Initialize();

	// �J�����X�V����
	void Update();

	void Set();
};
#pragma once
#include "Common.h"

// �萔�o�b�t�@�p�f�[�^�\���� (3D�ϊ��s��)
struct CameraConstBufferData
{
	XMMATRIX view; 
	XMMATRIX projection;
	XMMATRIX billboard;
	XMFLOAT3 position;
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
	// XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 target = { 0,0,0 };
	XMFLOAT3 position = { 0,0,-20 };

	// 3D�p�̎ˉe�ϊ�
	XMMATRIX matProjection = XMMatrixIdentity();

	// �r���[�ϊ��s��
	XMMATRIX matView = XMMatrixIdentity();
	XMFLOAT3 up = { 0,1,0 };
	
	// �r���{�[�h�s��
	XMMATRIX matBillboard = XMMatrixIdentity(); 

public:
	// �J����������
	void Initialize();

	// �J�����X�V����
	void Update();

	void Set();
};
#pragma once
#include "Sprite.h"
#include "Common.h"

class PostEffect : public Sprite
{
private: // �T�u�N���X

	struct VertexPosUv 
	{
		Float3 pos;	// xyz���W
		Float2 uv;	// uv ���W
	};

	struct ConstBufferData 
	{
		Float4 color;
		Matrix4 mat;
	};

private: // �ÓI�����o�ϐ�
	// ��ʃN���A�J���[
	static const float clearColor[4];

public:	// �����o�֐�
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	PostEffect();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �p�C�v���C������
	/// </summary>
	void CreateGraphicsPipelineState();

	/// <summary>
	/// �`��R�}���h�̔��s
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// �V�[���`��O����
	/// </summary>
	/// <param name="commandList">�R�}���h���X�g</param>
	void PreDrawScene(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// �V�[���`��㏈��
	/// </summary>
	/// <param name="commandList">�R�}���h���X�g</param>
	void PostDrawScene(ID3D12GraphicsCommandList* commandList);

private: // DirectX �|�C���^��
	// �o�b�t�@��
	ComPtr<ID3D12Resource> texBuff = nullptr;
	ComPtr<ID3D12Resource> constBuff = nullptr;
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	ComPtr<ID3D12Resource> depthBuff = nullptr;

	// �O���t�B�b�N�X�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState;

	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

	// SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV = nullptr;

	// RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV = nullptr;
	
	// DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV = nullptr;

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
};
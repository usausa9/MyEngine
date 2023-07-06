#include "PostEffect.h"
#include "WinAPI.h"
#include "DirectXBase.h"

PostEffect::PostEffect(TextureIndex tex) : Sprite(tex = tIndex,
	{ 100, 100 },	// pos
	{ 500.0f,500.0f },
	{ 1.0f,1.0f,1.0f,1.0f },
	{ 0.0f, 0.0f },
	false, false
)
{
}

void PostEffect::Initialize()
{
	HRESULT result;

	// ���N���X
	Sprite::Init();

	// �e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinAPI::Get()->width,
		(UINT)WinAPI::Get()->height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// �e�N�X�`���o�b�t�@�̐���
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(result));

	{
		// �e�N�X�`����ԃN���A
		// ��f��(1280 * 720 = 921600�s�N�Z��)
		const UINT pixelCount = WinAPI::Get()->width * WinAPI::Get()->height;
		// �摜1�s���̃f�[�^�T�C�Y
		const UINT rowPitch = sizeof(UINT) * WinAPI::Get()->width;
		// �摜�S�̂̃f�[�^�T�C�Y
		const UINT depthPitch = rowPitch * WinAPI::Get()->height;
		// �摜�C���[�W
		UINT* img = new UINT[pixelCount];
		for (unsigned int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff->WriteToSubresource(0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[] img;
	}

	// SRV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	// SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = DirectXBase::Get()->device->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	// SRV�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	// �ݒ�\����
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	// �f�X�N���v�^�q�[�v��SRV����
	DirectXBase::Get()->device->CreateShaderResourceView(texBuff.Get(),
		&srvDesc, 
		descHeapSRV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::Draw(ID3D12GraphicsCommandList* commandList)
{
	//ID3D12GraphicsCommandList* commandList = DirectXBase::Get()->commandList.Get();
	Update();

	// �f�X�N���v�^�q�[�v�̃Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());


	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// CBV�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetData(tIndex)->gpuHandle);
	
	// �`��R�}���h
	commandList->DrawInstanced(4, 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}
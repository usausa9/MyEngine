#include "PostEffect.h"
#include "WinAPI.h"
#include "DirectXBase.h"

// �ÓI�����o�ϐ��̎���
const float PostEffect::clearColor[4] = { 0.25f,0.5f,0.1f,1.0f }; // RGBA �΂��ۂ��F

PostEffect::PostEffect()
{ 

}

void PostEffect::Initialize()
{
	HRESULT result;

	// ���N���X
	// Sprite::Init();

	// ���_�f�[�^
	VertexPosUv vertices[]
	{
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
		{{-0.5f, +0.5f, 0.0f}, {0.0f, 0.0f}},
		{{+0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
		{{+0.5f, +0.5f, 0.0f}, {1.0f, 0.0f}},
	};

	// �o�b�t�@�p�q�[�v�ݒ�
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);

	// ���_�o�b�t�@�p�ݒ�
	CD3DX12_RESOURCE_DESC vertResDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * _countof(vertices));

	// ���_�o�b�t�@����
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&vertResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) 
	{
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̐ݒ�
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	// �萔�o�b�t�@�p���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC constResDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	// �萔�o�b�t�@����
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&constResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

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

	CD3DX12_CLEAR_VALUE clearValue(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);

	// �e�N�X�`���o�b�t�@�̐���
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
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

	// RTV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc = {};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 1;
	// RTV�p�f�X�N���v�^�q�[�v�𐶐�
	result = DirectXBase::Get()->device->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	// �V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// �f�X�N���v�^�q�[�v��RTV�쐬
	DirectXBase::Get()->device->CreateRenderTargetView(texBuff.Get(),
		&renderTargetViewDesc,
		descHeapRTV->GetCPUDescriptorHandleForHeapStart());

	// �[�x�o�b�t�@���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			WinAPI::Get()->width,
			WinAPI::Get()->height,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	// �q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	// �[�x�l�N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;		// �[�x�l1.0f(�ő�l)�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	// �[�x�l�t�H�[�}�b�g

	// �[�x�o�b�t�@�̐���
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));

	// DSV�p�f�X�N���v�^�q�[�v��ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
	DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescHeapDesc.NumDescriptors = 1;
	// DSV�p�f�X�N���v�^�q�[�v���쐬
	result = DirectXBase::Get()->device->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	// �f�X�N���v�^�q�[�v��DSV�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DirectXBase::Get()->device->CreateDepthStencilView(depthBuff.Get(),
		&dsvDesc,
		descHeapDSV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::Draw(ID3D12GraphicsCommandList* commandList)
{
	// �s��X�V�p�A�b�v�f�[�g
	//Update();

	HRESULT result;

	// �萔�o�b�t�@�Ƀf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = this->constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) 
	{
		constMap->color = this->color;
		constMap->mat.Identity();
		this->constBuff->Unmap(0, nullptr);
	}

	// �p�C�v���C���X�e�[�g�̐ݒ�
	commandList->SetPipelineState(pipelineState.Get());

	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };

	// SRV�̃Z�b�g
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// SRV�q�[�v�̐ݒ�R�}���h
	//commandList->SetDescriptorHeaps(1, TextureManager::srvHeap.GetAddressOf());

	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// CBV�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetData(tIndex)->gpuHandle);
	commandList->SetGraphicsRootDescriptorTable(1,descHeapSRV->GetGPUDescriptorHandleForHeapStart());

	// �`��R�}���h
	commandList->DrawInstanced(4, 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void PostEffect::PreDrawScene(ID3D12GraphicsCommandList* commandList)
{
	// ���\�[�X�o���A��ύX (�V�F�[�_�[���\�[�X���`��\)
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = texBuff.Get(); // �o�b�N�o�b�t�@���w��
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // �V�F�[�_�[���\�[�X����
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;			 // �`���Ԃ�
	commandList->ResourceBarrier(1, &barrierDesc);
	
	// �����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	// �����_�[�^�[�Q�b�g���Z�b�g
	commandList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// �r���[�|�[�g�ݒ�R�}���h
	D3D12_VIEWPORT viewport{};
	viewport.Width = (float)WinAPI::Get()->width;
	viewport.Height = (float)WinAPI::Get()->height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList->RSSetViewports(1, &viewport);

	// �V�U�[��`
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // �؂蔲�����W��
	scissorRect.right = scissorRect.left + WinAPI::Get()->width;  // �؂蔲�����W�E
	scissorRect.top = 0; // �؂蔲�����W��
	scissorRect.bottom = scissorRect.top + WinAPI::Get()->height; // �؂蔲�����W��
	// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList->RSSetScissorRects(1, &scissorRect);

	// �S��ʃN���A
	commandList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	// �[�x�o�b�t�@�̃N���A
	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene(ID3D12GraphicsCommandList* commandList)
{
	// ���\�[�X�o���A��ύX (�`��\���V�F�[�_�[���\�[�X)
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = texBuff.Get(); // �o�b�N�o�b�t�@���w��
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;        // �V�F�[�_�[���\�[�X��
	commandList->ResourceBarrier(1, &barrierDesc);
}
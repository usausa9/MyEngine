#include "PostEffect.h"

#include "DirectXBase.h"

PostEffect::PostEffect() : Sprite(
	100,
	{ 0, 0 },
	{ 500.0f, 500.0f },
	{ 1.0f,1.0f,1.0f,1.0f },
	{ 0.0f, 0.0f },
	false, false) // �㉺���E����
{

}

PostEffect::PostEffect(TextureIndex tex) : Sprite(tex = tIndex,
	{ 100, 100 },	// pos
	{ 1,1 },
	{ 1.0f,1.0f,1.0f,1.0f },
	{ 0.0f, 0.0f },
	false, false
)
{
	Init();
}

void PostEffect::Draw(ID3D12GraphicsCommandList* commandList)
{
	//ID3D12GraphicsCommandList* commandList = DirectXBase::Get()->commandList.Get();
	Update();

	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// CBV�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetData(tIndex)->gpuHandle);

	// �`��R�}���h
	commandList->DrawInstanced(4, 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}
#include "PostEffect.h"

#include "DirectXBase.h"

PostEffect::PostEffect() : Sprite(
	100,
	{ 0, 0 },
	{ 500.0f, 500.0f },
	{ 1.0f,1.0f,1.0f,1.0f },
	{ 0.0f, 0.0f },
	false, false) // 上下左右判定
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

	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// CBVの設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetData(tIndex)->gpuHandle);

	// 描画コマンド
	commandList->DrawInstanced(4, 1, 0, 0); // 全ての頂点を使って描画
}
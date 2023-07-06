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

	// 基底クラス
	Sprite::Init();

	// テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinAPI::Get()->width,
		(UINT)WinAPI::Get()->height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// テクスチャバッファの生成
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(result));

	{
		// テクスチャを赤クリア
		// 画素数(1280 * 720 = 921600ピクセル)
		const UINT pixelCount = WinAPI::Get()->width * WinAPI::Get()->height;
		// 画像1行分のデータサイズ
		const UINT rowPitch = sizeof(UINT) * WinAPI::Get()->width;
		// 画像全体のデータサイズ
		const UINT depthPitch = rowPitch * WinAPI::Get()->height;
		// 画像イメージ
		UINT* img = new UINT[pixelCount];
		for (unsigned int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

		// テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[] img;
	}

	// SRV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	// SRV用デスクリプタヒープを生成
	result = DirectXBase::Get()->device->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	// SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	// 設定構造体
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	// デスクリプタヒープにSRV生成
	DirectXBase::Get()->device->CreateShaderResourceView(texBuff.Get(),
		&srvDesc, 
		descHeapSRV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::Draw(ID3D12GraphicsCommandList* commandList)
{
	//ID3D12GraphicsCommandList* commandList = DirectXBase::Get()->commandList.Get();
	Update();

	// デスクリプタヒープのセット
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());


	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// CBVの設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetData(tIndex)->gpuHandle);
	
	// 描画コマンド
	commandList->DrawInstanced(4, 1, 0, 0); // 全ての頂点を使って描画
}
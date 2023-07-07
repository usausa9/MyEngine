#include "TextureManager.h"
#include "DirectXBase.h"

int TextureManager::srvIndex = 0;
D3D12_DESCRIPTOR_HEAP_DESC TextureManager::srvHeapDesc{};
ComPtr<ID3D12DescriptorHeap> TextureManager::srvHeap = nullptr;
ComPtr<ID3D12Resource> TextureManager::texBuff[maxTextureNum] = {};
TextureData TextureManager::textureData[maxTextureNum] = {};

void TextureManager::Init()
{
	HRESULT result = S_FALSE;

	srvIndex = 0;

	// デスクリプタヒープの設定
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダーから見えるように
	srvHeapDesc.NumDescriptors = maxTextureNum;

	// 設定をもとにSRV用デスクリプタヒープを生成
	result = DirectXBase::Get()->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	for (auto& buff : texBuff)
	{
		buff = nullptr;
	}

	for (auto& data : textureData)
	{
		data = {};
	}
}

TextureIndex TextureManager::Load(std::wstring filepath)
{
	HRESULT result = S_FALSE;

	if (srvIndex >= maxTextureNum)
	{
		OutputDebugString(L"over maxTextureNum");

		return -1; // GG
	}

	ScratchImage scratchImg{};
	// WICテクスチャのロード
	result = LoadFromWICFile(
		filepath.c_str(),
		WIC_FLAGS_NONE,
		&textureData[srvIndex].metadata, scratchImg);

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		textureData[srvIndex].metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	textureData[srvIndex].metadata.format = MakeSRGB(textureData[srvIndex].metadata.format);

	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM; // GPUの転送用
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = textureData[srvIndex].metadata.format;
	textureResourceDesc.Width = textureData[srvIndex].metadata.width;
	textureResourceDesc.Height = (UINT)textureData[srvIndex].metadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)textureData[srvIndex].metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)textureData[srvIndex].metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

#pragma region テクスチャバッファ

	// テクスチャバッファの生成
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff[srvIndex]));
	assert(SUCCEEDED(result));

	// 全ミップマップについて
	for (size_t i = 0; i < textureData[srvIndex].metadata.mipLevels; i++)
	{
		// ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		// テクスチャバッファにデータ転送
		result = texBuff[srvIndex]->WriteToSubresource(
			(UINT)i,
			nullptr,		// 全領域へコピー
			img->pixels,	// 元データアドレス
			(UINT)img->rowPitch,	// 1ラインサイズ
			(UINT)img->slicePitch	// 全サイズ
		);
		assert(SUCCEEDED(result));
	}

	// SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	srvHandle.ptr += 
		DirectXBase::Get()->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;

	// シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};		// 設定構造体
	srvDesc.Format = textureResourceDesc.Format;// RGBA float
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	// ハンドルの指す位置にシェーダーリソースビュー作成
	DirectXBase::Get()->device->CreateShaderResourceView(texBuff[srvIndex].Get(), &srvDesc, srvHandle);

	// GPU用
	D3D12_GPU_DESCRIPTOR_HANDLE gpuSrvHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	gpuSrvHandle.ptr +=
		DirectXBase::Get()->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;

	textureData[srvIndex].cpuHandle = srvHandle;
	textureData[srvIndex].gpuHandle = gpuSrvHandle;

	srvIndex++;

	return srvIndex - 1;
}

TextureIndex TextureManager::Load(std::string filepath)
{
	// SJIS → wstring
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, filepath.c_str()
		, -1, (wchar_t*)NULL, 0);

	// バッファの取得
	wchar_t* cpUCS2 = new wchar_t[iBufferSize];

	// SJIS → wstring
	MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, cpUCS2
		, iBufferSize);

	// stringの生成
	std::wstring wPath(cpUCS2, cpUCS2 + iBufferSize - 1);

	// バッファの破棄
	delete[] cpUCS2;

	return Load(wPath);
}

TextureData* TextureManager::GetData(TextureIndex index)
{
	return &textureData[index];
}

ID3D12Resource* TextureManager::GetBuff(TextureIndex index)
{
	return texBuff[index].Get();
}

void TextureManager::Release()
{
	for (auto& buff : texBuff)
	{
		if (buff.Get())
		{
			buff = ComPtr<ID3D12Resource>();
		}
	}

	for (auto& data : textureData)
	{
		data = {};
	}
}
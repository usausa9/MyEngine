#pragma once
#include "Common.h"
#include <DirectXTex.h>

#include <string>

typedef unsigned int TextureIndex;

struct TextureData
{
	TexMetadata metadata{};

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
};

class TextureManager
{
public:
	// èâä˙âª
	static void Init();
	static TextureIndex Load(std::wstring filepath);
	static TextureIndex Load(std::string filepath);

	static TextureData* GetData(TextureIndex index);
	static ID3D12Resource* GetBuff(TextureIndex index);

	static void Release();

private:
	const static int maxTextureNum = 1024;
	
	static int srvIndex;
	static D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
	

	static ComPtr<ID3D12Resource> texBuff[maxTextureNum];
	static TextureData textureData[maxTextureNum];

public:
	static ComPtr<ID3D12DescriptorHeap> srvHeap;
};
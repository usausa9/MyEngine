#include "Sprite.h"
#include "DirectXBase.h"
#include "SpriteManager.h"

#include "WinAPI.h"



// 頂点データ構造体
struct Vertex
{
	Float3 pos;	// xyz座標
	Float2 uv;	// uv座標
};

Sprite::Sprite()
{
	
}

Sprite::Sprite(TextureIndex tex) : tIndex(tex)
{
	Init();
}

Sprite::~Sprite()
{
	constBuffMaterial->Unmap(0, nullptr);	//  メモリリークは罪
}

void Sprite::Init()
{
#pragma region 頂点バッファ
	HRESULT pResult;

	float w = TextureManager::GetData(tIndex)->metadata.width / 2.f;
	float h = TextureManager::GetData(tIndex)->metadata.height / 2.f;

	// 頂点データ
	Vertex vertices[] =
	{
		// X  Y   Z        U     V
		{{-w,-h, 0.0f },  {0.0f, 0.0f}},	// 左下
		{{-w, h, 0.0f },  {0.0f, 1.0f}},	// 左上
		{{ w,-h, 0.0f },  {1.0f, 0.0f}},	// 右下
		{{ w, h, 0.0f },  {1.0f, 1.0f}},	// 右上
	};

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	pResult = DirectXBase::Get()->device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(pResult));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	pResult = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(pResult));

	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i]; // 座標をコピー
	}

	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);
	
	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);
#pragma endregion

#pragma region 定数バッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUの転送用

	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(SpriteConstBufferDataMaterial) + 0xff) & ~0xff; // 256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT vResult;

	// 定数バッファの生成
	vResult = DirectXBase::Get()->device->CreateCommittedResource(
		&cbHeapProp,	// ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,// リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(vResult));

	// 定数バッファのマッピング
	vResult = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial); // マッピング
	assert(SUCCEEDED(vResult));

	// 値を書き込むと自動的に転送される
	constMapMaterial->color = Float4{1.f, 0.f, 1.f, 1.f}; // RGBAで半透明の赤
	
	Matrix4 matWorld;
	matWorld = matWorld.Scale({ scale.x, scale.y, 1 });
	matWorld *= matWorld.RotateZ(rotation);
	matWorld *= matWorld.Translate({ position.x, position.y, 0 });

	constMapMaterial->mat = matWorld * SpriteManager::SpriteProjection;
	
#pragma endregion
}

void Sprite::Update() 
{
	Matrix4 matWorld;
	matWorld = matWorld.Scale({ scale.x, scale.y, 1 });
	matWorld *= matWorld.RotateZ(rotation);
	matWorld *= matWorld.Translate({ position.x, position.y, 0 });

	constMapMaterial->mat = matWorld * SpriteManager::SpriteProjection;
};

void Sprite::Draw()
{
	ID3D12GraphicsCommandList* commandList = DirectXBase::Get()->commandList.Get();

	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// CBVの設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetData(tIndex)->gpuHandle);

	// 描画コマンド
	commandList->DrawInstanced(4, 1, 0, 0); // 全ての頂点を使って描画
}
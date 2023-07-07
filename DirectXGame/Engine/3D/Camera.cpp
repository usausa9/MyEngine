#include "Camera.h"
#include "WinAPI.h"
#include "DirectXBase.h"

Camera::~Camera()
{
	constBuffCamera->Unmap(0, nullptr);	// メモリリークは罪
}

void Camera::Initialize()
{
	// 射影変換行列
	matProjection = matProjection.CreateProjectionMat(
		UsaMath::DegreesToRadians(90.0f),	// 上下画角90度
		(float)WinAPI::Get()->width / WinAPI::Get()->height,
		0.1f, 1000.0f
	);

	HRESULT result = S_FALSE;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUの転送用

	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(CameraConstBufferData) + 0xff) & ~0xff; // 256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&cbHeapProp,	// ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,// リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffCamera));
	assert(SUCCEEDED(result));

	// 定数バッファのマッピング
	result = constBuffCamera->Map(0, nullptr, (void**)&constMapCamera);
	assert(SUCCEEDED(result));
}

void Camera::Update()
{
	matView = Matrix4::Identity();
	matView = matView.CreateViewMat(position, target, up);

	constMapCamera->projection = matProjection;
	constMapCamera->view = matView;
	constMapCamera->position = position;

	matBillboard = Matrix4::Inverse(matView);
	matBillboard.m[3][0] = 0;
	matBillboard.m[3][1] = 0;
	matBillboard.m[3][2] = 0;
	matBillboard.m[3][3] = 1;

	constMapCamera->billboard = matBillboard;
}

void Camera::Set()
{
	// 定数バッファビューをセット [カメラ]
	DirectXBase::Get()->commandList->SetGraphicsRootConstantBufferView(3, constBuffCamera->GetGPUVirtualAddress());
}
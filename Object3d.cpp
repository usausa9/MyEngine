#include "Object3D.h"

#include "DirectXBase.h"

void Object3D::InitializeObject3D()
{
	HRESULT result;

	// 定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	// 定数バッファのリソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(constMapTransform) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));

	// 定数バッファのマッピング
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
	assert(SUCCEEDED(result));
}

// 3Dオブジェクト更新処理
void Object3D::UpdateObject3D()
{
	Matrix4 matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = matScale.Scale(scale);
	matRot = matRot.Identity();
	matRot *= matRot.RotateZ(rotation.z);
	matRot *= matRot.RotateX(rotation.x);
	matRot *= matRot.RotateY(rotation.y);
	matTrans = matTrans.Translate(position);

	// ワールド行列の合成
	matWorld = matWorld.Identity();	// 変形をリセット
	matWorld *= matScale;			// ワールド行列にスケーリングを反映
	matWorld *= matRot;				// ワールド行列に回転を反映
	matWorld *= matTrans;			// ワールド行列に平行移動を反映

	// 親オブジェクトがあれば
	if (parent != nullptr) 
	{
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}

	// 定数バッファへデータ転送
	constMapTransform->mat = matWorld;
}

void Object3D::DrawObject3D()
{
	// 定数バッファビュー(CBV)の設定コマンド
	DirectXBase::Get()->commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
	
	// 描画！
	model->Draw();
}
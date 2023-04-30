#include "ParticleManager.h"
#include "DirectXBase.h"

ComPtr<ID3D12RootSignature> ParticleManager::rootSignature; 	// ルートシグネチャ
ComPtr<ID3D12PipelineState> ParticleManager::pipelineState;	// パイプライン

// XMFLOAT3同士の加算処理
const XMFLOAT3 operator+(const XMFLOAT3& Ihs, const XMFLOAT3& rhs)
{
	XMFLOAT3 result;
	result.x = Ihs.x + rhs.x;
	result.y = Ihs.y + rhs.y;
	result.z = Ihs.z + rhs.z;
	return result;
}

void ParticleManager::CreatePipeline()
{
	HRESULT result;

	// デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;		// 一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;	// テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[4] = {};

	// 定数バッファ0番 [マテリアル]
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// 種類
	rootParams[0].Descriptor.ShaderRegister = 0;						// 定数バッファの番号
	rootParams[0].Descriptor.RegisterSpace = 0;							// デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// 全てのシェーダから見える

	// テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// 種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			// デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						// デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				// 全てのシェーダから見える

	// 定数バッファ1番 [行列]
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// 種類
	rootParams[2].Descriptor.ShaderRegister = 1;						// 定数バッファの番号
	rootParams[2].Descriptor.RegisterSpace = 0;							// デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// 全てのシェーダから見える

	// 定数バッファ2番 [カメラ]
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// 種類
	rootParams[3].Descriptor.ShaderRegister = 2;						// 定数バッファの番号
	rootParams[3].Descriptor.RegisterSpace = 0;							// デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// 全てのシェーダから見える

	ComPtr<ID3DBlob> vsBlob = nullptr;		 // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;		 // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob = nullptr;		 // ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;	 // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticleVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// エラーなら
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticlePS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	// エラーなら
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ジオメトリシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticleGS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "gs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&gsBlob, &errorBlob);

	// エラーなら
	if (FAILED(result))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ // xyz座標
			"POSITION",									// セマンティック名
			0,											// 同じセマンティック名が複数あるときに使うインデックス (0でおっけー)
			DXGI_FORMAT_R32G32B32_FLOAT,				// 要素数とビット数を表す (XYZの3つでfloat型なのでR32G32B32_FLOAT)
			0,											// 入力スロットインデックス (0でおっけー)
			D3D12_APPEND_ALIGNED_ELEMENT,				// データのオフセット値 (D3D12_APPEND_ALIGNED_ELEMENTだと自動設定
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // 入力データ種別 (標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT)
			0											// 一度に描画するインスタンス数 (0でおっけー)
		}, // 座標以外に色、テクスチャUVなどを渡す場合は更に続ける。

		{ // uv座標
			"TEXCOORD",									// セマンティック名
			0,											// 同じセマンティック名が複数あるときに使うインデックス (0でおっけー)
			DXGI_FORMAT_R32G32_FLOAT,				    // 要素数とビット数を表す (uvの2つでfloat型なのでR32G32_FLOAT)
			0,											// 入力スロットインデックス (0でおっけー)
			D3D12_APPEND_ALIGNED_ELEMENT,				// データのオフセット値 (D3D12_APPEND_ALIGNED_ELEMENTだと自動設定
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // 入力データ種別 (標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT)
			0											// 一度に描画するインスタンス数 (0でおっけー)
		}, // 座標以外に色、テクスチャUVなどを渡す場合は更に続ける。

		{ // カラー
			"COLOR",									// セマンティック名
			0,											// 同じセマンティック名が複数あるときに使うインデックス (0でおっけー)
			DXGI_FORMAT_R32G32B32A32_FLOAT,				// 要素数とビット数を表す (XYZの3つでfloat型なのでR32G32B32_FLOAT)
			0,											// 入力スロットインデックス (0でおっけー)
			D3D12_APPEND_ALIGNED_ELEMENT,				// データのオフセット値 (D3D12_APPEND_ALIGNED_ELEMENTだと自動設定
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // 入力データ種別 (標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT)
			0											// 一度に描画するインスタンス数 (0でおっけー)
		}, // 座標以外に色、テクスチャUVなどを渡す場合は更に続ける。
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();
	pipelineDesc.GS.pShaderBytecode = gsBlob->GetBufferPointer();
	pipelineDesc.GS.BytecodeLength = gsBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面をカリング
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	//pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME; // ポリゴン内を塗りつぶさない場合はこっち(ワイヤーフレーム)
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	// ブレンドステート
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画
	// 加算
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 横繰り返し(タイリング)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 縦繰り返し(タイリング)
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 奥行繰り返し(タイリング)
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;		// 全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;	// ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;				// ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// ピクセルシェーダーからのみ使用可能

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;	// ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);		// ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = DirectXBase::Get()->device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	// rootSigBlob->Release();

	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();

	// デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;							// 深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;// 書き込み不可！！！！！！！！！！
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		// 小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;		// 深度値フォーマット

	// パイプランステートの生成
	result = DirectXBase::Get()->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}

// パーティクル初期化
void ParticleManager::InitializeParticle()
{
	HRESULT result;

	{
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

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertexCount);

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapprop{};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	// 頂点データ リソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeVB;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	DirectXBase::Get()->device->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	// PIXデバッグ用の名前付け
	vertBuff->SetName(L"PARTICLE VERT BUFF");

	// GPU上のバッファに対応した仮想メモリを取得
	vertBuff->Map(0, nullptr, (void**)&vertMap);

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);
}

// パーティクル更新処理
void ParticleManager::UpdateParticle()
{
	// 寿命が尽きたパーティクルを全削除
	particles.remove_if(
		[](Particle& x) {
			return x.frame >= x.num_frame;
		}
	);

	// 全パーティクル更新
	for (forward_list<Particle>::iterator it = particles.begin(); it != particles.end(); it++) 
	{
		// 経過フレーム数カウント
		it->frame++;
		it->velocity = it->velocity + it->accel;
		it->position = it->position + it->velocity;

		float f = (float)it->frame / it->num_frame;

		it->scale = (it->e_scale - it->s_scale) * f;
		it->scale += it->s_scale;
	}

	// 生存数を0で初期化
	activeCount = 0;

	// 頂点バッファへデータ転送
	// パーティクルの情報を1つずつ反映
	for (forward_list<Particle>::iterator it = particles.begin(); it != particles.end(); it++)
	{
		it->Update();

		vertMap[activeCount].pos = it->position;
		vertMap[activeCount].scale = XMFLOAT2(it->scale, it->scale);
		vertMap[activeCount].color = it->color;

		activeCount++;
	}
}

// パーティクル描画
void ParticleManager::DrawParticle(TextureIndex index)
{
	// 頂点バッファビューの設定
	DirectXBase::Get()->commandList->IASetVertexBuffers(0, 1, &vbView);

	// 定数バッファビュー(CBV)の設定コマンド
	DirectXBase::Get()->commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

	// シェーダリソースビューをセット
	DirectXBase::Get()->commandList->
		SetGraphicsRootDescriptorTable
		(1, TextureManager::GetData(index)->gpuHandle);

	// 描画コマンド
	DirectXBase::Get()->commandList->
		DrawInstanced((UINT)activeCount, 1, 0, 0);
}

void ParticleManager::Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale)
{
	// リストに要素追加
	particles.emplace_front();
	// 追加した要素の参照
	Particle& p = particles.front();
	// 値のセット
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.num_frame = life;
	p.scale = start_scale;
	p.s_scale = start_scale;
	p.e_scale = end_scale;
}
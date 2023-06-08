#include "MyGame.h"

using namespace Input;

// 定数バッファ用データ構造体 (3D変換行列)
struct ConstBufferDataTransform
{
	Matrix4 mat; // 3D変換行列
};

void MyGame::Initialize()
{
#ifdef _DEBUG
	// デバッグレイヤーをオンに
	ID3D12Debug1* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

#pragma region 基盤システムの初期化
	
	window.Init();
	window.CreateWindowObj(L"UsaEngine", 1280, 720);
	window.Show();

	// DirectX初期化
	DirectXBase::Get()->Init();

	// DirectInputの初期化
	Key::Init(window.w.hInstance, window.hwnd);
	Pad::Init();

	// スプライト共通部の初期化	
	spriteManager = make_unique<SpriteManager>();
	spriteManager->Init();

#pragma endregion

#pragma region 描画初期化処理
	HRESULT result;
	{
		// ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

		// リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	}

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
	ComPtr<ID3DBlob> errorBlob = nullptr;	 // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicVS.hlsl", // シェーダファイル名
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
		L"Resources/shaders/BasicPS.hlsl", // シェーダファイル名
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

		{ // 法線ベクトル
			"NORMAL",									// セマンティック名
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
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

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

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//   横繰り返し(タイリング)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//   縦繰り返し(タイリング)
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
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	// 書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		// 小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;		// 深度値フォーマット

	// パイプランステートの生成
	
	result = DirectXBase::Get()->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	TextureManager::Init();
	FbxLoader::GetInstance()->Initialize(DirectXBase::Get()->device.Get());

	scene.Initialize();
}

void MyGame::Finalize()
{
	FbxLoader::GetInstance()->Finalize();
	TextureManager::Release();

	scene.Finalize();
}

void MyGame::Update()
{
#pragma region 基盤システムの更新
	if (window.ProcessMessage() == true)
	{
		endRequest_ = true;
	}

	// 入力の更新
	Key::Update();
	Pad::Update();
#pragma endregion

	scene.Update();
}

void MyGame::PreDraw()
{
	// パイプラインステートとルートシグネチャの設定コマンド
	DirectXBase::Get()->commandList->SetPipelineState(pipelineState.Get());
	DirectXBase::Get()->commandList->SetGraphicsRootSignature(rootSignature.Get());

	// プリミティブ形状の設定コマンド
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);	 // 点のリスト
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);		 // 線のリスト
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);	 // 線のストリップ
	DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	 // 三角形のリスト
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形のストリップ

	// SRVヒープの設定コマンド
	DirectXBase::Get()->commandList->SetDescriptorHeaps(1, TextureManager::srvHeap.GetAddressOf());

	//DirectXBase::Get()->commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetData(reimuTex)->gpuHandle);

	// DirectX 描画前処理
	DirectXBase::Get()->PreDraw();
}

void MyGame::PreDrawParticle()
{
	// パイプラインステートとルートシグネチャの設定コマンド
	DirectXBase::Get()->commandList->SetPipelineState(ParticleManager::pipelineState.Get());
	DirectXBase::Get()->commandList->SetGraphicsRootSignature(ParticleManager::rootSignature.Get());

	// プリミティブ形状の設定コマンド
	DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		 // 点のリスト
	//DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);		 // 線のリスト
	//DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);	 // 線のストリップ
	//DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	 // 三角形のリスト
	//DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形のストリップ

	// SRVヒープの設定コマンド
	DirectXBase::Get()->commandList->SetDescriptorHeaps(1, TextureManager::srvHeap.GetAddressOf());
}

void MyGame::Draw()
{
	// 描画前処理
	PreDraw();

	// 3D描画
	scene.Draw3D();

	// パーティクル描画前処理 + 描画処理
	PreDrawParticle();
	scene.DrawParticle();

	// 2D描画
	spriteManager->PreDraw();
	scene.Draw2D();

	// 描画後処理
	PostDraw();
}

void MyGame::PostDraw()
{
	// DirectX 描画後処理
	DirectXBase::Get()->PostDraw();
}

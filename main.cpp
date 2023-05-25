#pragma region include��

#include "Input.h"
#include "WinAPI.h"
#include "DirectXBase.h"

#include "Object3D.h"
#include "Camera.h"

#include "FbxLoader.h"

#include "Sprite.h"
#include "SpriteManager.h"

#include "Model.h"

#include "Common.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4.h"

#include "Collision.h"
#include "CollisionPrimitive.h"

#include "ParticleManager.h"

#include "FbxLoader.h"

#include <Windows.h>
#include <cmath>

#include <DirectXTex.h>

#pragma endregion

using namespace Input;

// �萔�o�b�t�@�p�f�[�^�\���� (3D�ϊ��s��)
struct ConstBufferDataTransform
{
	Matrix4 mat; // 3D�ϊ��s��
};

// Windows�A�v���ł̃G���g���[�|�C���g (main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	// �f�o�b�O���C���[���I����
	ID3D12Debug1* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

#pragma region ��ՃV�X�e���̏�����
	// WindowsAPI������
	WinAPI& window = *WinAPI::Get();	
	window.Init();
	window.CreateWindowObj(L"UsaEngine", 1280, 720);
	window.Show();
	
	// DirectX������
	DirectXBase::Get()->Init();

	// DirectInput�̏�����
	Key::Init(window.w.hInstance, window.hwnd);
	Pad::Init();

	// �X�v���C�g���ʕ��̏�����
	SpriteManager* spriteManager = nullptr;
	spriteManager = new SpriteManager;
	spriteManager->Init();

#pragma endregion

#pragma region �`�揉��������
	HRESULT result;
	{
		// �q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

		// ���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	}

	// �f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;		// ��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;	// �e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[4] = {};

	// �萔�o�b�t�@0�� [�}�e���A��]
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// ���
	rootParams[0].Descriptor.ShaderRegister = 0;						// �萔�o�b�t�@�̔ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;							// �f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// �S�ẴV�F�[�_���猩����

	// �e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// ���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			// �f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						// �f�t�H���g�l
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				// �S�ẴV�F�[�_���猩����

	// �萔�o�b�t�@1�� [�s��]
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// ���
	rootParams[2].Descriptor.ShaderRegister = 1;						// �萔�o�b�t�@�̔ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;							// �f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// �S�ẴV�F�[�_���猩����

	// �萔�o�b�t�@2�� [�J����]
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// ���
	rootParams[3].Descriptor.ShaderRegister = 2;						// �萔�o�b�t�@�̔ԍ�
	rootParams[3].Descriptor.RegisterSpace = 0;							// �f�t�H���g�l
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// �S�ẴV�F�[�_���猩����

	ComPtr<ID3DBlob> vsBlob = nullptr;		 // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob = nullptr;		 // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob = nullptr;	 // �G���[�I�u�W�F�N�g
	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicVS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicPS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ // xyz���W
			"POSITION",									// �Z�}���e�B�b�N��
			0,											// �����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X (0�ł������[)
			DXGI_FORMAT_R32G32B32_FLOAT,				// �v�f���ƃr�b�g����\�� (XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT)
			0,											// ���̓X���b�g�C���f�b�N�X (0�ł������[)
			D3D12_APPEND_ALIGNED_ELEMENT,				// �f�[�^�̃I�t�Z�b�g�l (D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // ���̓f�[�^��� (�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT)
			0											// ��x�ɕ`�悷��C���X�^���X�� (0�ł������[)
		}, // ���W�ȊO�ɐF�A�e�N�X�`��UV�Ȃǂ�n���ꍇ�͍X�ɑ�����B

		{ // �@���x�N�g��
			"NORMAL",									// �Z�}���e�B�b�N��
			0,											// �����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X (0�ł������[)
			DXGI_FORMAT_R32G32B32_FLOAT,				// �v�f���ƃr�b�g����\�� (XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT)
			0,											// ���̓X���b�g�C���f�b�N�X (0�ł������[)
			D3D12_APPEND_ALIGNED_ELEMENT,				// �f�[�^�̃I�t�Z�b�g�l (D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // ���̓f�[�^��� (�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT)
			0											// ��x�ɕ`�悷��C���X�^���X�� (0�ł������[)
		}, // ���W�ȊO�ɐF�A�e�N�X�`��UV�Ȃǂ�n���ꍇ�͍X�ɑ�����B

		{ // uv���W
			"TEXCOORD",									// �Z�}���e�B�b�N��
			0,											// �����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X (0�ł������[)
			DXGI_FORMAT_R32G32_FLOAT,				    // �v�f���ƃr�b�g����\�� (uv��2��float�^�Ȃ̂�R32G32_FLOAT)
			0,											// ���̓X���b�g�C���f�b�N�X (0�ł������[)
			D3D12_APPEND_ALIGNED_ELEMENT,				// �f�[�^�̃I�t�Z�b�g�l (D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // ���̓f�[�^��� (�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT)
			0											// ��x�ɕ`�悷��C���X�^���X�� (0�ł������[)
		}, // ���W�ȊO�ɐF�A�e�N�X�`��UV�Ȃǂ�n���ꍇ�͍X�ɑ�����B
	};

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// �V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // �w�ʂ��J�����O
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
	//pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME; // �|���S������h��Ԃ��Ȃ��ꍇ�͂�����(���C���[�t���[��)
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	// �u�����h�X�e�[�g
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��

	// ���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//   ���J��Ԃ�(�^�C�����O)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//   �c�J��Ԃ�(�^�C�����O)
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// ���s�J��Ԃ�(�^�C�����O)
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// �{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;		// �S�ă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;	// �~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;				// �~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// �s�N�Z���V�F�[�_�[����̂ݎg�p�\
	
	// ���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;

	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;	// ���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);		// ���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = DirectXBase::Get()->device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	// rootSigBlob->Release();

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature;

	// �f�v�X�X�e���V���X�e�[�g�̐ݒ�
	pipelineDesc.DepthStencilState.DepthEnable = true;							// �[�x�e�X�g���s��
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	// �������݋���
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		// ��������΍��i
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;		// �[�x�l�t�H�[�}�b�g

	// �p�C�v�����X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	result = DirectXBase::Get()->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	// ���s����

	ParticleManager::CreatePipeline();
	TextureManager::Init();

	TextureIndex circleTex = TextureManager::Load(L"Resources/circleLight.png");
	TextureIndex thunderTex = TextureManager::Load(L"Resources/thunder.png");
	
	TextureIndex reimuTex = TextureManager::Load(L"Resources/reimu.png");
	TextureIndex doraTex = TextureManager::Load(L"Resources/texture.png");
	Sprite* sprite = new Sprite(doraTex);
	Sprite* sprite2 = new Sprite(reimuTex);

	ParticleManager circleParticle;
	ParticleManager thunderParticle;

	circleParticle.InitializeParticle();
	thunderParticle.InitializeParticle();

	Model icoModel= Model::LoadFromOBJ("Ico");
	Model cubeModel = Model::LoadFromOBJ("Cube");
	
	Object3D ico;
	Object3D cube;

	Camera camera;

	ico.position = { 2.5f,0,0 }; 

	float icoRad = 1.f;
	ico.scale = { icoRad,icoRad,icoRad };
	cube.scale = { 1.f,1.f,1.f };

	ico.InitializeObject3D();
	cube.InitializeObject3D();

	camera.Initialize();

	ico.model = &icoModel;
	cube.model = &cubeModel;

#pragma endregion

	FbxLoader::GetInstance()->Initialize(DirectXBase::Get()->device.Get());
	FbxLoader::GetInstance()->LoadModelFromFile("cube");

	Matrix4 a = 
	{
		1.f,2.f,2.f,2.f,
		2.f,1.f,2.f,2.f,
		2.f,2.f,1.f,2.f,
		2.f,2.f,2.f,1.f
	};

	// �Q�[�����[�v
	while (true)
	{
#pragma region ��ՃV�X�e���̍X�V
		if (window.ProcessMessage() == true)
		{
			break;
		}
		// ���͂̍X�V
		Key::Update();
		Pad::Update();
#pragma endregion

		// DIrectX���t���[������(�X�V����) ��������
		ico.UpdateObject3D();
		cube.UpdateObject3D();

		a.Inverse();

		for (int i = 0; i < 15; i++) 
		{
			const float rnd_pos = 10.0f;
			Float3 pos{};
			pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f - 10.f;
			pos.y = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
			pos.z = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;

			const float rnd_vel = 0.1f;
			Float3 vel{};
			vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
			vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
			vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

			Float3 acc{};
			const float rnd_acc = 0.1f;
			// acc.y = -(float)rand() / RAND_MAX * rnd_acc;

			thunderParticle.Add(40, pos, vel, acc, 1.5f, 0.0f);
		}

		for (int i = 0; i < 15; i++)
		{
			const float rnd_pos = 10.0f;
			Float3 pos{};
			pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f + 10.f;
			pos.y = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
			pos.z = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;

			const float rnd_vel = 0.1f;
			Float3 vel{};
			vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f ;
			vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
			vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

			Float3 acc{};
			const float rnd_acc = 0.1f;
			// acc.y = -(float)rand() / RAND_MAX * rnd_acc;

			circleParticle.Add(60, pos, vel, acc, 1.5f, 0.0f);
		}

		sprite->position = { 130,130 };
		sprite->Update();

		sprite2->position = { 1050,190 };
		sprite2->Update();

		if (Key::Down(DIK_A) || Key::Down(DIK_LEFT))
		{
			camera.position.x -= 0.5f;
		}
		else if (Key::Down(DIK_D) || Key::Down(DIK_RIGHT))
		{
			camera.position.x += 0.5f;
		}

		if (Key::Down(DIK_S) || Key::Down(DIK_DOWN))
		{
			camera.position.y -= 0.5f;
		}
		else if (Key::Down(DIK_W) || Key::Down(DIK_UP))
		{
			camera.position.y += 0.5f;
		}

		camera.Update();
		thunderParticle.UpdateParticle();
		circleParticle.UpdateParticle();

		// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
		DirectXBase::Get()->commandList->SetPipelineState(pipelineState.Get());
		DirectXBase::Get()->commandList->SetGraphicsRootSignature(rootSignature);

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);	 // �_�̃��X�g
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);		 // ���̃��X�g
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);	 // ���̃X�g���b�v
		DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	 // �O�p�`�̃��X�g
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`�̃X�g���b�v

		// SRV�q�[�v�̐ݒ�R�}���h
		DirectXBase::Get()->commandList->SetDescriptorHeaps(1, TextureManager::srvHeap.GetAddressOf());

		//DirectXBase::Get()->commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetData(reimuTex)->gpuHandle);

		// DirectX �`��O����
		DirectXBase::Get()->PreDraw();

		camera.Set();

#pragma region 3D�I�u�W�F�N�g�`��
	
		ico.DrawObject3D();
		cube.DrawObject3D();
		
#pragma endregion

#pragma region �p�[�e�B�N���`��

		// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
		DirectXBase::Get()->commandList->SetPipelineState(ParticleManager::pipelineState.Get());
		DirectXBase::Get()->commandList->SetGraphicsRootSignature(ParticleManager::rootSignature.Get());

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);		 // �_�̃��X�g
		//DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);		 // ���̃��X�g
		//DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);	 // ���̃X�g���b�v
		//DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	 // �O�p�`�̃��X�g
		//DirectXBase::Get()->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`�̃X�g���b�v

		// SRV�q�[�v�̐ݒ�R�}���h
		DirectXBase::Get()->commandList->SetDescriptorHeaps(1, TextureManager::srvHeap.GetAddressOf());
		
		camera.Set();
		circleParticle.DrawParticle(circleTex);
		thunderParticle.DrawParticle(thunderTex);

#pragma endregion

		// �X�v���C�g�`��O����
		spriteManager->PreDraw();
#pragma region �X�v���C�g�`��

		sprite->Draw();
		sprite2->Draw();

#pragma endregion

		// DirectX �`��㏈��
		DirectXBase::Get()->PostDraw();
	}

	delete spriteManager;
	delete sprite;
	delete sprite2;

	FbxLoader::GetInstance()->Finalize();
	TextureManager::Release();

	return 0;
}

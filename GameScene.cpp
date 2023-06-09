#include "GameScene.h"

#include "FBXObject3D.h"

using namespace Input;

void GameScene::Initialize()
{

	// �f�o�C�X���Z�b�g
	FBXObject3D::SetDevice(DirectXBase::Get()->device.Get());
	// �J�������Z�b�g
	FBXObject3D::SetCamera(camera);
	// �R�}���h���X�g������
	FBXObject3D::SetCommandList(DirectXBase::Get()->commandList.Get());
	// �O���t�B�b�N�X�p�C�v���C������
	FBXObject3D::CreateGraphicsPipeline();

	// ���f�������w�肵�ăt�@�C���ǂݍ���
	model1 = FbxLoader::GetInstance()->LoadModelFromFile("cube");

	circleTex = TextureManager::Load(L"Resources/circleLight.png");
	thunderTex = TextureManager::Load(L"Resources/thunder.png");
	reimuTex = TextureManager::Load(L"Resources/reimu.png");
	doraTex = TextureManager::Load(L"Resources/texture.png");

	sprite = make_unique<Sprite>(doraTex);
	sprite2 = make_unique<Sprite>(reimuTex);

	ParticleManager::CreatePipeline();

	circleParticle.InitializeParticle();
	thunderParticle.InitializeParticle();

	icoModel = Model::LoadFromOBJ("Ico");
	cubeModel = Model::LoadFromOBJ("Cube");

	ico.position = { 2.5f,0,0 };
	ico.scale = { icoRad,icoRad,icoRad };

	cube.scale = { 1.f,1.f,1.f };

	ico.InitializeObject3D();
	cube.InitializeObject3D();

	ico.model = &icoModel;
	cube.model = &cubeModel;

	// FBX���f���֘A
	object1 = new FBXObject3D;
	object1->Initialize();
	object1->SetModel(model1);

	// �J����������
	camera->Initialize();

	camera->target = { 0,20,0 };
	camera->position = { 0,0,-100 };
}

void GameScene::Finalize()
{
	delete object1;
	delete model1;
	delete camera;
}

void GameScene::Update()
{
	// DIrectX���t���[������(�X�V����) ��������
	ico.UpdateObject3D();
	cube.UpdateObject3D();

	// �p�[�e�B�N��
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
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
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
		camera->position.x -= 0.5f;
	}
	else if (Key::Down(DIK_D) || Key::Down(DIK_RIGHT))
	{
		camera->position.x += 0.5f;
	}

	if (Key::Down(DIK_S) || Key::Down(DIK_DOWN))
	{
		camera->position.y -= 0.5f;
	}
	else if (Key::Down(DIK_W) || Key::Down(DIK_UP))
	{
		camera->position.y += 0.5f;
	}

	
	object1->Update();

	camera->Update();
	thunderParticle.UpdateParticle();
	circleParticle.UpdateParticle();
}

void GameScene::Draw3D()
{
	// �J�����Z�b�g
	camera->Set();

	// 3D�I�u�W�F�`��
	ico.DrawObject3D();
	cube.DrawObject3D();

	object1->Draw();
}

void GameScene::DrawParticle()
{
	// �J�����Z�b�g
	camera->Set();

	// �p�[�e�B�N���I�u�W�F�`��
	circleParticle.DrawParticle(circleTex);
	thunderParticle.DrawParticle(thunderTex);
}

void GameScene::Draw2D()
{
	sprite->Draw();
	sprite2->Draw();
}
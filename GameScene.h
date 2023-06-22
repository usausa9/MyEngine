#pragma once
#include "Input.h"
#include "ParticleManager.h"
#include "Sprite.h"
#include "DirectXBase.h"

#include "FBXObject3D.h"
#include "Object3D.h"
#include "Camera.h"

#include "OBJModel.h"
#include "FBXModel.h"

#include "FbxLoader.h"

class GameScene
{
private:
	// �e�N�X�`��
	//TextureIndex circleTex;
	//TextureIndex thunderTex;
	//TextureIndex reimuTex;
	//TextureIndex doraTex;

	// �X�v���C�g
	//unique_ptr<Sprite> sprite = nullptr;
	//unique_ptr<Sprite> sprite2 = nullptr;

	// �p�[�e�B�N��
	//ParticleManager circleParticle;
	//ParticleManager thunderParticle;

	// OBJ���f��
	OBJModel icoModel;
	OBJModel cubeModel;

	// Object3D
	//Object3D ico;
	//Object3D cube;
	
	// FBX
	//FBXModel* model1 = nullptr;
	FBXModel* boneTestModel = nullptr;
	//FBXObject3D* object1 = nullptr;
	FBXObject3D* boneTestObject = nullptr;

	// �J����
	Camera* camera = new Camera;

	//float icoRad = 1.f;

public:
	// ������
	void Initialize();

	// �I������
	void Finalize();

	// ���t���[���X�V
	void Update();

	// 3D�`��
	void Draw3D();

	// �p�[�e�B�N���`��
	void DrawParticle();

	// 2D�`��
	void Draw2D();
};
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
	// テクスチャ
	//TextureIndex circleTex;
	//TextureIndex thunderTex;
	//TextureIndex reimuTex;
	//TextureIndex doraTex;

	// スプライト
	//unique_ptr<Sprite> sprite = nullptr;
	//unique_ptr<Sprite> sprite2 = nullptr;

	// パーティクル
	//ParticleManager circleParticle;
	//ParticleManager thunderParticle;

	// OBJモデル
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

	// カメラ
	Camera* camera = new Camera;

	//float icoRad = 1.f;

public:
	// 初期化
	void Initialize();

	// 終了処理
	void Finalize();

	// 毎フレーム更新
	void Update();

	// 3D描画
	void Draw3D();

	// パーティクル描画
	void DrawParticle();

	// 2D描画
	void Draw2D();
};
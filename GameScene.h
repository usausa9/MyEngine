#pragma once
#include "Input.h"
#include "ParticleManager.h"
#include "Sprite.h"
#include "DirectXBase.h"

#include "FBXObject3D.h"
#include "Object3D.h"
#include "Camera.h"

#include "FbxLoader.h"

class GameScene
{
private:
	TextureIndex circleTex;
	TextureIndex thunderTex;
	TextureIndex reimuTex;
	TextureIndex doraTex;

	unique_ptr<Sprite> sprite = nullptr;
	unique_ptr<Sprite> sprite2 = nullptr;

	ParticleManager circleParticle;
	ParticleManager thunderParticle;

	Model icoModel;
	Model cubeModel;

	Object3D ico;
	Object3D cube;
	
	Model* model1 = nullptr;
	FBXObject3D* object1 = nullptr;

	Camera* camera = new Camera;

	float icoRad = 1.f;

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
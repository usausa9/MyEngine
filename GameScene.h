#pragma once
#include "Input.h"
#include "ParticleManager.h"
#include "Sprite.h"

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

	Camera camera;

	float icoRad = 1.f;

public:
	// 初期化
	void Initialize();

	// 毎フレーム更新
	void Update();

	// 3D描画
	void Draw3D();

	// パーティクル描画
	void DrawParticle();

	// 2D描画
	void Draw2D();
};
#pragma once
#include "Common.h"
#include "Vector3.h"
#include "Model.h"
#include "Camera.h"

// 定数バッファ用データ構造体 (3D変換行列)
struct ParticleConstBufferDataTransform
{
	Matrix4 mat; // 3D変換行列
};

struct Vertex
{
	Float3 pos;
	Float2 scale;
	Float4 color;
};

struct Particle
{
	Float3 position = {};	// 座標
	Float3 velocity = {}; // 速度
	Float3 accel = {};	// 加速度
	int frame = 0;			// 現在フレーム
	int num_frame = 0;		// 終了フレーム
	Float4 color = { 1,1,1,1 };

	float scale = 1.0f;

	float s_scale = 1.0f;
	float e_scale = 0.0f;

	void Update() 
	{
		float t = (float)frame / num_frame;
		float t2 = 1 - t;

		//color = { t2,0,t,t2 };
	};
};

class ParticleManager
{
public:
	// 定数バッファ (行列用)
	ID3D12Resource* constBuffTransform;
	
	// 頂点データセット
	const int vertexCount = 1024;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	Vertex* vertMap = nullptr;

	// 定数バッファマップ (行列用)
	ParticleConstBufferDataTransform* constMapTransform;

	// アフィン変換情報
	Vector3 scale = { 1,1,1 };
	Vector3 rotation = { 0,0,0 };
	Vector3 position = { 0,0,0 };

public:
	static ComPtr<ID3D12RootSignature> rootSignature; 	// ルートシグネチャ
	static ComPtr<ID3D12PipelineState> pipelineState;	// パイプライン

	// パイプライン作成
	static void CreatePipeline();

	// particle初期化
	void InitializeParticle();

	// particle更新処理
	void UpdateParticle();

	// particle描画処理
	void DrawParticle(TextureIndex index);

	// コンテナに追加するもの
	void Add(int life, Float3 position, Float3 velocity, Float3 accel, float start_scale, float end_scale);

private:
	int activeCount = 0;
	forward_list<Particle> particles;
};
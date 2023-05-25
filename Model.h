#pragma once
#include "TextureManager.h"
#include "Common.h"

#include <DirectXTex.h>

// 定数バッファ用 データ構造体 (マテリアル)
struct ConstBufferDataMaterial
{
	Float3 ambient;
	float pad1;

	Float3 diffuse;
	float pad2;

	Float3 specular;
	float alpha;

	Float4 color; // 色(RGBA)
};

struct Node
{
	// 名前
	string name;

	// ローカルスケール
	Vector4 scaling = { 1,1,1,0 };
	// ローカル回転角
	Vector4 rotation = { 0,0,0,0 };
	// ローカル移動
	Vector4 translation = { 0,0,0,1 };
	// ローカル変形行列
	Matrix4 transform;
	// グローバル変形行列
	Matrix4 globalTransform;

	// 親ノード
	Node* parent = nullptr;
};

class Model
{
public:
	// フレンドクラス
	friend class FbxLoader;

private:
	// モデル名
	string name;
	// ノード配列
	vector<Node> nodes;

public:
	Float3 ambient = { 1,1,1 };
	Float3 diffuse = { 1,1,1 };
	TexMetadata metadata = {};
	ScratchImage scratchImg = {};

	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		Float3 pos;	// xyz座標
		Float3 normal;// 法線ベクトル 
		Float2 uv;	// uv座標
	};

	// メッシュを持つノード
	Node* meshNode = nullptr;
	// 頂点データ配列
	vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	vector<unsigned short> indices;

	// マテリアル
	struct Material
	{
		string name;
		Float3 ambient;
		Float3 diffuse;
		Float3 specular;
		float alpha;

		TextureIndex index = -1;

		Material()
		{
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};	

	Material material;

public:
	Model();

public:
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	// OBJファイルから３Dモデル読み込み
	static Model LoadFromOBJ(const std::string& modelName);

	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	void CreateBuffers();

	void Draw();
		
private:
	void LoadFromOBJInternal(const std::string& modelName);

private:
	//std::vector<unsigned short> indices;
	//std::vector<VertexPosNormalUv> vertices;

	ComPtr<ID3D12Resource> vertBuff = nullptr;
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
};
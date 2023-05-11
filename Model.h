#pragma once
#include "TextureManager.h"
#include "Common.h"

// 定数バッファ用 データ構造体 (マテリアル)
struct ConstBufferDataMaterial
{
	XMFLOAT3 ambient;
	float pad1;

	XMFLOAT3 diffuse;
	float pad2;

	XMFLOAT3 specular;
	float alpha;

	XMFLOAT4 color; // 色(RGBA)
};

struct Node
{

};

class Model
{
public:
	// フレンドクラス
	friend class FbxLoader;

private:
	// モデル名
	std::string name;

public:
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos;	// xyz座標
		XMFLOAT3 normal;// 法線ベクトル 
		XMFLOAT2 uv;	// uv座標
	};

	// マテリアル
	struct Material
	{
		std::string name;
		XMFLOAT3 ambient;
		XMFLOAT3 diffuse;
		XMFLOAT3 specular;
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
	std::vector<unsigned short> indices;
	std::vector<VertexPosNormalUv> vertices;

	ComPtr<ID3D12Resource> vertBuff = nullptr;
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
};
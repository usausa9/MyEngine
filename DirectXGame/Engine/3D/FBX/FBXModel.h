#pragma once
#include "TextureManager.h"
#include "Common.h"
#include "DirectXBase.h"

#include <DirectXTex.h>
#include <fbxsdk.h>

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

class FBXModel
{
private:
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // 定数
	// ボーンインデックスの最大数
	static const int MAX_BONE_INDICES = 4;

public:
	// フレンドクラス
	friend class FBXLoader;

	// ボーン構造体
	struct Bone
	{
		// 名前
		std::string name;
		// 初期姿勢の逆行列
		Matrix4 invInitialPose;
		// クラスター (FBX側のボーン情報)
		FbxCluster* fbxCluster;
		// コンストラクタ
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};

	// 頂点データ構造体
	struct VertexPosNormalUvSkin
	{
		Float3 pos;		// xyz座標
		Float3 normal;	// 法線ベクトル 
		Float2 uv;		// uv座標
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};

private:
	// モデル名
	string name;
	// ノード配列
	vector<Node> nodes;
	// メッシュを持つノード
	Node* meshNode = nullptr;
	// 頂点データ配列
	vector<VertexPosNormalUvSkin> vertices;
	// 頂点インデックス配列
	vector<unsigned short> indices;
	// アンビエント係数
	Float3 ambient = { 1,1,1 };
	// ディフューズ係数
	Float3 diffuse = { 1,1,1 };
	// テクスチャメタデータ
	TexMetadata metadata = {};
	// スクラッチイメージ
	ScratchImage scratchImg = {};
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};
	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV = nullptr;
	// ボーン配列
	std::vector<Bone> bones;
	// FBXシーン
	FbxScene* fbxScene = nullptr;

public:
	// デストラクタ
	~FBXModel();

	// バッファ生成
	void CreateBuffers(ID3D12Device* device);

	// 描画
	void Draw(ID3D12GraphicsCommandList* commandList);

	// モデルの変形行列取得
	const Matrix4& GetModelTransform() { return meshNode->globalTransform; }

	// getter
	std::vector<Bone>& GetBones() { return bones; }

	// getter
	FbxScene* GetFbxScene() { return fbxScene; }
};
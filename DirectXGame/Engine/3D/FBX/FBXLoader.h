#pragma once
#include "fbxsdk.h"
#include "Common.h"

#include "FBXModel.h"

#include <d3dx12.h>

#include <cassert>
#include <string>

class FBXLoader
{
public: // 定数
	// モデル格納ルートパス
	static const string baseDirectory; 

	// テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFilename;

private:
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FBXLoader& obj) = delete;

	// ディレクトリを含んだファイルパスからファイル名を抽出する
	string ExtractFileName(const string& path);

	// privateなコンストラクタ（シングルトンパターン）
	FBXLoader() = default;

	// privateなデストラクタ（シングルトンパターン）
	~FBXLoader() = default;

	// コピーコンストラクタを禁止（シングルトンパターン）
	FBXLoader(const FBXLoader& obj) = delete;

private:
	// D3D12デバイス
	ID3D12Device* device = nullptr;

	// FBXマネージャ
	FbxManager* fbxManager = nullptr;

	// FBXインポータ
	FbxImporter* fbxImporter = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">D3D12デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 後始末
	/// </summary>
	void Finalize();

	/// <summary>
	/// ファイルからFBXモデル読み込み
	/// </summary>
	/// <param name="modelName">モデル名</param>
	FBXModel* LoadModelFromFile(const string& modelName);

	/// <summary>
	/// 再帰的にノード構成を解析
	/// </summary>
	/// <param name="model">読み込み先オブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseNodeRecursive(FBXModel* model, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseMesh(FBXModel* model, FbxNode* fbxNode);

	// 頂点座標読み取り
	void ParseMeshVertices(FBXModel* model, FbxMesh* fbxMesh);
	// 面情報読み取り
	void ParseMeshFaces(FBXModel* model, FbxMesh* fbxMesh);
	// マテリアル読み取り
	void ParseMaterial(FBXModel* model, FbxNode* fbxNode);
	// テクスチャ読み取り
	void LoadTexture(FBXModel* model, const string& fullpath);
	// スキニング情報読み取り
	void ParseSkin(FBXModel* model, FbxMesh* fbxMesh);

	/// <summary>
	/// FBXの行列をMatrix4へ変換
	/// </summary>
	/// <param name="dst">書き込み先</param>
	/// <param name="src">読み込むFBX行列</param>
	static void ConvertMatrixFromFBX(Matrix4* dst, const FbxAMatrix& src);

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FBXLoader* GetInstance();
};
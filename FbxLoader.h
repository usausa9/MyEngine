﻿#pragma once
#include "fbxsdk.h"
#include "Common.h"

#include <d3dx12.h>

#include <cassert>
#include <string>

class FbxLoader
{
private: // エイリアス
	// std::を省略
	using string = std::string;

public: // 定数
	// モデル格納ルートパス
	static const string baseDirectory; 

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;

	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;

	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;

	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

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
	void LoadModelFromFile(const string& modelName);

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();
};
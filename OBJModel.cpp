#include "OBJModel.h"

#include "DirectXBase.h"

#include <sstream>
#include <fstream>

using namespace std;
//Model::Material Model::material;

//ID3D12Device* Model::device = nullptr;

OBJModel::OBJModel()
{

}

OBJModel OBJModel::LoadFromOBJ(const std::string& modelName)
{
	// Model型のインスタンスメモリ確保
	OBJModel model = OBJModel();

	// 読み込み
	model.LoadFromOBJInternal(modelName);

	// ばっふぁ生成
	model.CreateBuffers();

	return model;
#pragma endregion
}

void OBJModel::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	ifstream file;
	// ﾏﾃﾘｱﾙファイルを開く
	file.open(directoryPath + filename);

	// ファイルオープン失敗をチェック
	if (file.fail()) { assert(0); }

	string line;

	while (getline(file, line))
	{
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::stringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');

		// 先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin());	// 先頭削除
		}

		// 先頭が newmtl ならマテリアル名
		if (key == "newmtl") {
			// マテリアル読み込み
			line_stream >> material.name;
		}

		// 先頭が Ka ならアンビエント色
		if (key == "Ka") {
			// マテリアル読み込み
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}

		// 先頭が Kd ならディフューズ色
		if (key == "Ka") {
			// マテリアル読み込み
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}

		// 先頭が Ks ならスペキュラー色
		if (key == "Ks") {
			// マテリアル読み込み
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}

		// 先頭が map_Kdならテクスチャファイル名
		if (key == "map_Kd") {

			string texFPath;

			// テクスチャ
			line_stream >> texFPath;

			// テクスチャのロード
			material.index = TextureManager::Load(directoryPath + texFPath);
		}
	}
	// ファイルを閉じる
	file.close();
}

void OBJModel::CreateBuffers()
{
	HRESULT result;
#pragma region 頂点バッファ

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices.size());

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	// 頂点データ リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// 全頂点に対して
	for (int i = 0; i < vertices.size(); i++)
	{
		vertMap[i] = vertices[i]; // 座標をコピー
	}

	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();

	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;

	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(VertexPosNormalUv);

#pragma endregion
#pragma region インデックスバッファ

	// インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices.size());

	// インデックスデータ リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // インデックスデータ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// インデックスバッファの生成
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	// インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	// 全インデックスに対して
	for (int i = 0; i < indices.size(); i++) {
		indexMap[i] = indices[i]; //インデックスをコピー
	}

	// インデックス マッピング解除
	indexBuff->Unmap(0, nullptr);

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

#pragma endregion

#pragma region 定数バッファ

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUの転送用

	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; // 256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	result = DirectXBase::Get()->device->CreateCommittedResource(
		&cbHeapProp,	// ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,// リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

#pragma region マテリアルの転送
	// 定数バッファのマッピング
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial); // マッピング
	assert(SUCCEEDED(result));

	constMapMaterial->ambient = material.ambient;
	constMapMaterial->diffuse = material.diffuse;
	constMapMaterial->specular = material.specular;
	constMapMaterial->alpha = material.alpha;

	constMapMaterial->color = { 1,1,1,1 };

	// constBuffMaterial->Unmap(0, nullptr);
#pragma endregion
}

void OBJModel::Draw()
{
	// 頂点バッファビューの設定
	DirectXBase::Get()->commandList->IASetVertexBuffers(0, 1, &vbView);
	DirectXBase::Get()->commandList->IASetIndexBuffer(&ibView);

	// 定数バッファビューをセット（マテリアル）
	DirectXBase::Get()->commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	// シェーダリソースビューをセット
	DirectXBase::Get()->commandList->
		SetGraphicsRootDescriptorTable
		(1, TextureManager::GetData(material.index)->gpuHandle);


	// 描画コマンド
	DirectXBase::Get()->commandList->
		DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}

void OBJModel::LoadFromOBJInternal(const std::string& modelName)
{
	ifstream file;

	// OBJからのデータ読み込み
	const string fileName = modelName + ".obj";
	const string directoryPath = "Resources/Models/" + modelName + "/";

	file.open(directoryPath + fileName); //"Resources/Models/ICO/ICO.obj"

	// ファイルオープン失敗をチェック
	if (file.fail()) { assert(0); }

	vector<XMFLOAT3> positions; // 頂点座標
	vector<XMFLOAT3> normals; // 法線ベクトル
	vector<XMFLOAT2> texcoords; // テクスチャUV

	string line;

	while (getline(file, line))
	{
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');

		// 先頭文字列がmtllibならマテリアル
		if (key == "mtllib")
		{
			// マテリアルのファイル名読み込み
			std::string filename;
			line_stream >> filename;

			// マテリアル読み込み
			LoadMaterial(directoryPath, filename);
		}

		// 先頭文字列がvなら頂点座標
		if (key == "v") {
			// X,Y,Z座標読み込み
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			// 座標データに追加
			positions.emplace_back(position);
		}

		// 先頭文字列がvtならテクスチャ
		if (key == "vt") {
			// U,V成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			// V方向反転
			texcoord.y = 1.0f - texcoord.y;

			// テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}

		// 先頭文字列がVNなら法線ベクトル
		if (key == "vn") {
			// X,Y,Z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			// 法線ベクトルデータに追加
			normals.emplace_back(normal);
		}

		// 先頭文字列がfならポリゴン（三角形）
		if (key == "f") {
			// 半角スペース区切りで行の続きを読み込む
			std::string index_string;
			while (getline(line_stream, index_string, ' ')) {

				// 頂点インデックス１個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, std::ios_base::cur);// スラッシュを飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, std::ios_base::cur);// スラッシュを飛ばす
				index_stream >> indexNormal;

				// 頂点データの追加
				VertexPosNormalUv vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				vertices.emplace_back(vertex);

				// インデックスデータの追加
				indices.emplace_back((unsigned short)indices.size());
			}
		}
	}
	// ファイルを閉じる
	file.close();
}
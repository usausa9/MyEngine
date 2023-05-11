#include "FbxLoader.h"

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string FbxLoader::baseDirectory = "Resources/";

void FbxLoader::Initialize(ID3D12Device* device)
{
    // 再初期化チェック
    assert(fbxManager == nullptr);

    // 引数からメンバ変数に代入
    this->device = device;

    // FBXマネージャの生成
    fbxManager = FbxManager::Create();

    // FBXマネージャの入出力設定
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(ios);

    // FBXインポータの生成
    fbxImporter = FbxImporter::Create(fbxManager, "");
}

void FbxLoader::Finalize()
{
    // 各種FBXインスタンスの破棄
    fbxImporter->Destroy();
    fbxManager->Destroy();
}

void FbxLoader::LoadModelFromFile(const string& modelName)
{
    // モデルと同じ名前のフォルダから読み込む
    const string directoryPath = baseDirectory + modelName + "/";

    // 拡張子.fbxを付加
    const string fileName = modelName + ".fbx";

    // 連結してフルパスを得る
    const string fullpath = directoryPath + fileName;

    // ファイル名を指定してFBXファイルを読み込む
    if (!fbxImporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings()))
    {
        assert(0);
    }

    // シーン生成
    FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");

    // ファイルからロードしたFBXの情報をシーンにインポート
    fbxImporter->Import(fbxScene);

    // モデル生成
    Model* model = new Model();
    model->name = modelName;

    // ルートノードから順に解析してモデルに流し込む
    ParseNodeRecursive(model, fbxScene->GetRootNode());

    // FBXシーン解放
    fbxScene->Destroy();
}

void FbxLoader::ParseNodeRecursive(Model* model, FbxNode* fbxNode)
{
    // ノード名を取得
    string name = fbxNode->GetName();
    // モデルにノードを追加 (TD)
    // FBXノードの情報を解析してノードに記録 (TD)
    // FBXノードのメッシュ情報を解析 (TD)

    // 子ノードに対して再帰呼び出し
    for (int i = 0; i < fbxNode->GetChildCount(); i++)
    {
        ParseNodeRecursive(model, fbxNode->GetChild(i));
    }
}

FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}
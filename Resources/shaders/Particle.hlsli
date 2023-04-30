

//cbuffer cbuff0 : register(b0)
//{
//    matrix mat; // ３Ｄ変換行列
//    matrix matBillboard; // ビルボード行列
//};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 pos : POSITION; // システム用頂点座標
    float scale : TEXCOORD;
    float4 color : COLOR;
	//float3 normal :NORMAL; // 法線ベクトル
	//float2 uv  :TEXCOORD; // uv値
};

// ジオメトリシェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct GSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
	//float3 normal :NORMAL; // 法線ベクトル
    float2 uv : TEXCOORD; // uv値
    float4 color : COLOR;
};

// 虚無
cbuffer ConstBufferDataMaterial : register(b0)
{
    // レジスタを合わせるために虚無を取得
};

// 3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
	// 3D変換行列
    matrix world;
};

// カメラ
cbuffer ConstBufferDataCamera : register(b2)
{
    matrix view;
    matrix projection;
    matrix billboard;
    float3 cameraPos;
};

//// 頂点シェーダーの出力構造体
//// (頂点シェーダーからピクセルシェーダーへのやり取りに使用する)
//struct VSOutput
//{
//    float4 svpos : SV_POSITION; // システム用頂点座標
//    float4 worldpos : POSITION; // ワールド座標
//    float3 normal : NORMAL; // 法線ベクトル
//    float2 uv : TEXCOORD; // uv値
//};
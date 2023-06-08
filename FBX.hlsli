cbuffer ConstBuffer0 : register(b0)
{
	matrix viewProjection;	// ビュープロジェクション行列
	matrix worldTransform;	// ワールド行列
	float3 cameraPosition;	// カメラ座標(ワールド座標)
};

// 頂点バッファの入力
struct VSInput
{
	float4 pos : POSITION;	// 位置
	float3 normal : NORMAL;	// 頂点法線
	float2 uv : TEXCOORD;	// テクスチャ法線
};

// 頂点シェーダーの出力構造体
// (頂点シェーダーからピクセルシェーダーへのやり取りに使用する)
struct VSOutput
{
	float4 svpos : SV_POSITION;	// システム用頂点座標
	float3 normal : NORMAL;		// 法線
	float2 uv : TEXCOORD;		// uv値
};

// マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
	// 色(RGBA)
    float4 color;
    matrix mat;
};


// 頂点シェーダーの出力構造体
// (頂点シェーダーからピクセルシェーダーへのやり取りに使用する)
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};
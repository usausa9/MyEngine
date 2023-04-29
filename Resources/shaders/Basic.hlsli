// マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
    float3 hAmbient : packoffset(c0); // アンビエント係数
									
    float3 hDiffuse : packoffset(c1); // ディフューズ係数
									
    float3 hSpecular : packoffset(c2); // スペキュラー係数
    float alpha : packoffset(c2.w); // アルファ
	
	// // 色(RGBA)
	float4 color : packoffset(c3);
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

// 頂点シェーダーの出力構造体
// (頂点シェーダーからピクセルシェーダーへのやり取りに使用する)
struct VSOutput
{ 
	float4 svpos : SV_POSITION;	// システム用頂点座標
    float4 worldpos : POSITION; // ワールド座標
	float3 normal : NORMAL;		// 法線ベクトル
	float2 uv : TEXCOORD;		// uv値
};
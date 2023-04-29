#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);	// 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);		// 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	// テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
	// 仮のライト
    float3 lightv = normalize(float3(1, -1, 1)); 
	// シェーディング色
    float4 shadecolor;
	// 光沢度
    const float shininess = 4.0f;
	// 頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
	// ライトに向かうベクトルと法線の内積
    float3 dotlightnormal = dot(lightv, input.normal);
	// 反射光ベクトル
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
	// 環境反射光
    float3 ambient = hAmbient;
	// 拡散反射光
    float3 diffuse = dotlightnormal * hDiffuse;
	// 鏡面反射光
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * hSpecular;
	// 全て加算する
    shadecolor.rgb = (ambient + diffuse + specular);
    shadecolor.a = alpha;

	// シェーディング色で描画
    return shadecolor * texcolor * color;
}
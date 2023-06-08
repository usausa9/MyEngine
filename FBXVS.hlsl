#include "FBX.hlsli"

// エントリーポイント
VSOutput main(VSInput input) 
{
	// 法線にワールド行列によるスケーリング、回転を適用
	float4 wNormal = normalize(mul(world, float4(input.normal, 0)));
	// ピクセルシェーダに渡す値
	VSOutput output;
	// 行列による座標変換
	output.svpos = mul(mul(viewProjection, world), input.pos);
	// ワールド法線を次のステージに渡す
	output.normal = wNormal.xyz;
	// 入力値をそのまま次のステージに渡す
	output.uv = input.uv;

	return output;
}
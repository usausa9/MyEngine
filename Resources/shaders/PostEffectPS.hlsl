#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0);	// 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1);	// 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);		// 0番スロットに設定されたサンプラー

// エントリーポイント
float4 main(VSOutput input) : SV_TARGET
{
	float4 colortex0 = tex0.Sample(smp, input.uv);	// レンダーテクスチャ0

	// UVの画面1pixel当たりの変化量
	float convertU = 1.f / 1280.f;
	float convertV = 1.f / 720.f;

	// 分割数
	int divNum = 11;

	// 縞模様にする前の背景色の処理 色反転
	float4 color = { 1 - colortex0.rgb, 1 };	// tex0 の色反転 { 1 - r, 1 - g, 1 - b, 1 }

	// 縞模様として色をかぶせる部分の処理 平均ぼかし
	if (fmod(input.uv.y, 0.1f) < 0.05f)
	{
		color = (0, 0, 0, 0);

		for (int i = 0; i < divNum; i++)
		{
			for (int j = 0; j < divNum; j++)
			{
				color += tex1.Sample(smp,
					input.uv - float2(convertU * 5, convertV * 5) + float2(convertU * i, convertV * j));
			}
		}
		color.rgb /= (divNum * divNum);
	}

	return float4(color.rgb, 1);
}
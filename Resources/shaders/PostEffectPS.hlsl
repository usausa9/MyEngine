#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    // UV指定したピクセルの色をサンプリング
    float4 texColor = tex.Sample(smp, input.uv);
    //float4 texColor = tex.Sample(smp, input.uv + float2(0.5f, 0.0f));

    // 出力
    return float4(texColor.rgb, 1);
}
#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
    // UV�w�肵���s�N�Z���̐F���T���v�����O
    float4 texColor = tex.Sample(smp, input.uv);
    //float4 texColor = tex.Sample(smp, input.uv + float2(0.5f, 0.0f));

    // �o��
    return float4(texColor.rgb, 1);
}
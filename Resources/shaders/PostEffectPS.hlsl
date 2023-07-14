#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1 : register(t1);	// 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);		// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

// �G���g���[�|�C���g
float4 main(VSOutput input) : SV_TARGET
{
	float4 colortex0 = tex0.Sample(smp, input.uv);	// �����_�[�e�N�X�`��0

	// UV�̉��1pixel������̕ω���
	float convertU = 1.f / 1280.f;
	float convertV = 1.f / 720.f;

	// ������
	int divNum = 11;

	// �Ȗ͗l�ɂ���O�̔w�i�F�̏��� �F���]
	float4 color = { 1 - colortex0.rgb, 1 };	// tex0 �̐F���] { 1 - r, 1 - g, 1 - b, 1 }

	// �Ȗ͗l�Ƃ��ĐF�����Ԃ��镔���̏��� ���ςڂ���
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
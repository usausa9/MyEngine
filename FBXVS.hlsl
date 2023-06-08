#include "FBX.hlsli"

// �G���g���[�|�C���g
VSOutput main(VSInput input) 
{
	// �@���Ƀ��[���h�s��ɂ��X�P�[�����O�A��]��K�p
	float4 wNormal = normalize(mul(world, float4(input.normal, 0)));
	// �s�N�Z���V�F�[�_�ɓn���l
	VSOutput output;
	// �s��ɂ����W�ϊ�
	output.svpos = mul(mul(viewProjection, world), input.pos);
	// ���[���h�@�������̃X�e�[�W�ɓn��
	output.normal = wNormal.xyz;
	// ���͒l�����̂܂܎��̃X�e�[�W�ɓn��
	output.uv = input.uv;

	return output;
}
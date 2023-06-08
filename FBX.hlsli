cbuffer ConstBuffer0 : register(b0)
{
	matrix viewProjection;	// �r���[�v���W�F�N�V�����s��
	matrix worldTransform;	// ���[���h�s��
	float3 cameraPosition;	// �J�������W(���[���h���W)
};

// ���_�o�b�t�@�̓���
struct VSInput
{
	float4 pos : POSITION;	// �ʒu
	float3 normal : NORMAL;	// ���_�@��
	float2 uv : TEXCOORD;	// �e�N�X�`���@��
};

// ���_�V�F�[�_�[�̏o�͍\����
// (���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����)
struct VSOutput
{
	float4 svpos : SV_POSITION;	// �V�X�e���p���_���W
	float3 normal : NORMAL;		// �@��
	float2 uv : TEXCOORD;		// uv�l
};

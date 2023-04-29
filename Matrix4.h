#pragma once
#include "Vector3.h"

/// �s��
class Matrix4 
{
public:
	// �s��
	float m[4][4];

	// �R���X�g���N�^
	Matrix4();

	// �������w�肵�Ă̐���

	Matrix4
	(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	);

	// ������Z�q�I�[�o�[���[�h
	Matrix4& operator*=(const Matrix4& m2);
};

// �s��̃X�J���[�{
Matrix4 ScalarProduct(Matrix4 matrix, Vector3 scalar);

// �s�񓯎m�̊|���Z
Matrix4 Matrix4Product(Matrix4 matrix, Matrix4 matrix2);

// �P�ʍs����
Matrix4 IdentityMatrix(Matrix4 matrix);
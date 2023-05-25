#include "Matrix4.h"

#include <cmath>

// 単位行列代入
Matrix4 Matrix4::Identity()
{
	static const Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	*this = result;
	return *this;
}

Matrix4 Matrix4::Identity(Matrix4 matrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (j = i)
			{
				matrix.m[j][i] = 1;
			}
			else
			{
				matrix.m[j][i] = 0;
			}
		}
	}

	return matrix;
}

// 行列のスカラー
Matrix4 Matrix4::ScalarProduct(Matrix4 matrix, Vector3 scalar)
{
	matrix.m[0][0] *= scalar.x;
	matrix.m[1][0] *= scalar.x;
	matrix.m[2][0] *= scalar.x;
	matrix.m[3][0] *= scalar.x;

	matrix.m[0][1] *= scalar.y;
	matrix.m[1][1] *= scalar.y;
	matrix.m[2][1] *= scalar.y;
	matrix.m[3][1] *= scalar.y;

	matrix.m[0][2] *= scalar.z;
	matrix.m[1][2] *= scalar.z;
	matrix.m[2][2] *= scalar.z;
	matrix.m[3][2] *= scalar.z;

	return matrix;
}

// 行列同士の掛け算
Matrix4 Matrix4::Matrix4Product(Matrix4 matrix, Matrix4 matrix2)
{
	Matrix4 matResult;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matResult.m[j][i] =
				matrix.m[j][0] * matrix.m[i][0] +
				matrix.m[j][1] * matrix.m[i][1] +
				matrix.m[j][2] * matrix.m[i][2] +
				matrix.m[j][3] * matrix.m[i][3];
		}
	}

	return matResult;
}

// 拡大縮小行列
Matrix4 Matrix4::Scale(const Vector3& s)
{
	Matrix4 result
	{
		s.x , 0.0f, 0.0f, 0.0f,
		0.0f, s.y , 0.0f, 0.0f,
		0.0f, 0.0f, s.z , 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

#pragma region 回転行列

Matrix4 Matrix4::RotateX(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos , sin , 0.0f,
		0.0f, -sin, cos , 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

Matrix4 Matrix4::RotateY(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		cos , 0.0f, -sin, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin , 0.0f, cos , 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

Matrix4 Matrix4::RotateZ(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		cos , sin , 0.0f, 0.0f,
		-sin, cos , 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

#pragma endregion

Matrix4 Matrix4::Translate(const Vector3& t)
{
	Matrix4 result
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		t.x , t.y , t.z , 1.0f
	};

	return result;
}

Vector3 Matrix4::Transform(const Vector3& v, const Matrix4& m)
{
	float w = 
	v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	Vector3 result
	{
		(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
		(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
		(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w
	};

	return result;
}

Matrix4 Matrix4::Inverse()
{
	// i = 行  j = 列  k = 対角成分が存在する列

	Matrix4 mat;
	Matrix4 result;
	Matrix4 identityMatrix;

	mat = *this;

	// identityMatrixを単位行列で初期化
	identityMatrix = Identity();

	// 掃き出し用 右側にでた4*4が結果になる 右側に単位行列を代入
	float sweep[4][8];

	// 行列の左側に元の行列
	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++)
		{
			sweep[i][j] = mat.m[i][j];
			sweep[i][(j + 4)] = identityMatrix.m[i][j];
		}
	}

	// 逆行列計算
	for (int k = 0; k < 4; k++) 
	{
		// ゼロ除算対策
		double max = fabs(sweep[k][k]);
		int max_i = k;

		for (int i = k + 1; i < 4; i++) 
		{
			if (fabs(sweep[i][k]) > max) 
			{
				max = fabs(sweep[i][k]);
				max_i = i;
			}
		}

		// 絶対値の最大値が0なら逆行列が無いので単位行列を返すとする。
		if (fabs(sweep[max_i][k]) <= 0.000001f) 
		{
			return identityMatrix;
		}

		// 入れ替え
		if (k != max_i)
		{
			for (int i = 0; i < 8; i++) 
			{
				// 入れ替え元のtempに、max_i を代入
				float temp = sweep[max_i][i];

				// コピーしたので、k i を代入
				sweep[max_i][i] = sweep[k][i];

				// tempに格納しておいた元のデータを k iに代入
				sweep[k][i] = temp;
			}
		}

		// [k][k]に掛けたら1になる値を求める (ゼロ除算対策済み)
		float identityScalar = 1 / sweep[k][k];
		for (int i = 0; i < 8; i++) 
		{
			sweep[k][i] *= identityScalar;
		}

		// [i][k]が0になるようにk行を -i倍
		for (int i = 0; i < 4; i++) 
		{
			// 単位行列位置なら飛ばす
			if (i == k) { continue; }

			// k行目に掛ける値を求める
			float constantNum = -sweep[i][k];

			for (int j = 0; j < 8; j++) 
			{
				// これでi k が0になる
				sweep[i][j] += sweep[k][j] * constantNum;
			}
		}
	}

	// 完成
	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++) 
		{
			result.m[i][j] = sweep[i][j + 4];
		}
	}

	*this = result;
	return *this;
}

Matrix4 Matrix4::CreateViewMat(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	Matrix4 result;

	// カメラのワールド行列を作成
	Vector3 cameraVecZ = target - eye;
	cameraVecZ.Normalize();

	Vector3 cameraVecX = up.Cross(cameraVecZ);
	cameraVecX.Normalize();

	Vector3 cameraVecY = cameraVecZ.Cross(cameraVecX);
	cameraVecY.Normalize();

	result = 
	{
		cameraVecX.x,cameraVecX.y,cameraVecX.z,	0,
		cameraVecY.x,cameraVecY.y,cameraVecY.z,	0,
		cameraVecZ.x,cameraVecZ.y,cameraVecZ.z,	0,
		eye.x		,eye.y		 ,eye.z		  ,	1
	};

	// 作成したワールド行列を逆行列に変換
	result.Inverse();

	return result;
}

Matrix4 Matrix4::CreateProjectionMat(float fovY, float aspectRatio, float nearZ, float farZ)
{
	Matrix4 result;

	float h = 1 / tanf(fovY / 2.0f);
	float w = h / aspectRatio;
	float z = farZ / (farZ - nearZ);
	float cameraZ = (-nearZ * farZ) / (farZ - nearZ);

	result = 
	{
		w,0,0,0,
		0,h,0,0,
		0,0,z,1,
		0,0,cameraZ,0
	};

	return result;
}

Matrix4& Matrix4::operator*=(const Matrix4& m2)
{
	Matrix4 result{};

	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++) 
			{
				result.m[i][j] += m[i][k] * m2.m[k][j];
			}
		}
	}

	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = result.m[i][j];
		}
	}

	*this = result;
	return *this;
}

const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result = m1;

	return result *= m2;
}

const Vector3 operator*(const Vector3& v, const Matrix4& m)
{
	Matrix4 result;

	return result.Transform(v, m);
}
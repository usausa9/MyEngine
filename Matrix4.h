#pragma once

#include "UsaMath.h"

/// 行列
class Matrix4 
{
public:
	// 行列
	float m[4][4];

	// 単位行列代入
	Matrix4 Identity();
	Matrix4 Identity(Matrix4 matrix);

	// 行列のスカラー
	Matrix4 ScalarProduct(Matrix4 matrix, Vector3 scalar);
	
	// 行列同士の掛け算
	Matrix4 Matrix4Product(Matrix4 matrix, Matrix4 matrix2);
	
	// 拡大縮小行列
	Matrix4 Scale(const Vector3& s);
	
	// 回転行列
	Matrix4 RotateX(float angle);
	Matrix4 RotateY(float angle);
	Matrix4 RotateZ(float angle);
	
	// 平行移動行列作成
	Matrix4 Translate(const Vector3& t);

	// 座標変換（ベクトルと行列の掛け算）
	Vector3 Transform(const Vector3& v, const Matrix4& m);

	// 逆行列生成
	Matrix4 Inverse();

	// ビュー行列関連
	Matrix4 CreateViewMat(const Vector3& eye, const Vector3& target, const Vector3& up);
	Matrix4 CreateProjectionMat(float fovY, float aspectRatio, float nearZ, float farZ);

	// 代入演算子オーバーロード
	Matrix4& operator*=(const Matrix4& m2);
};

// 2項演算子オーバーロード
const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);
const Vector3 operator*(const Vector3& v, const Matrix4& m);
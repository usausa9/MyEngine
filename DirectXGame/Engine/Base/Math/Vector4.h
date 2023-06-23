#pragma once

class Vector4
{
public:
	// メンバ変数
	float x;	// x成分
	float y;	// y成分
	float z;	// z成分
	float w;	// w成分

public:
	// コンストラクタ
	Vector4();										// 零ベクトルとする
	Vector4(float x, float y, float z, float w);	// 各成分を指定して生成

	float Length()const;
	Vector4& Normalize();
	float Dot(const Vector4& v);

	// 単項演算子オーバーロード
	Vector4 operator+() const;
	Vector4 operator-() const;

	// 代入演算子オーバーロード
	Vector4& operator+=(const Vector4& v);
	Vector4& operator-=(const Vector4& v);
	Vector4& operator*=(float s);
	Vector4& operator/=(float s);
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応するため、以下のように準備している
const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator*(float s, const Vector4& v);
const Vector4 operator/(const Vector4& v, float s);
#include "Vector4.h"
#include <cmath>	// sqrt

// コンストラクタ
// 零ベクトルとして生成
Vector4::Vector4() : x(0), y(0), z(0), w(0) {}

// x,y成分を指定しての生成
Vector4::Vector4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {}

// ノルム(長さ)を求める
float Vector4::Length() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

// 正規化
Vector4& Vector4::Normalize() /*const*/
{
	float len = Length();

	if (len != 0) {
		return *this /= len;
	}

	return *this;
}

// 内積を求める
float Vector4::Dot(const Vector4& v)
{
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

// 単項演算子オーバーロード
Vector4 Vector4::operator+() const
{
	return *this;
}

Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

// 代入演算子オーバーロード
Vector4& Vector4::operator+=(const Vector4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;

	return *this;
}
Vector4& Vector4::operator-=(const Vector4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return*this;
}
Vector4& Vector4::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return*this;
}
Vector4& Vector4::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return*this;
}

// 2項演算子オーバーロード
const Vector4 operator+(const Vector4& v1, const Vector4& v2)
{
	Vector4 temp(v1);
	return temp += v2;
}
const Vector4 operator-(const Vector4& v1, const Vector4& v2)
{
	Vector4 temp(v1);
	return temp -= v2;
}
const Vector4 operator*(const Vector4& v, float s)
{
	Vector4 temp(v);
	return temp *= s;
}
const Vector4 operator*(float s, const Vector4& v)
{
	return v * s;
}
const Vector4 operator/(const Vector4& v, float s)
{
	Vector4 temp(v);
	return temp /= s;
}
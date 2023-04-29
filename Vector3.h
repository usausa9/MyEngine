#pragma once
#include <DirectXMath.h>

const float PI = 3.141592654f;

using namespace DirectX;

class Vector3
{
public:
	// メンバ変数
	float x; // x成分
	float y; // y成分
	float z; // z成分

public:
	// コンストラクタ
	Vector3();							// 零ベクトルとして生成
	Vector3(float x, float y, float z);	// x,y成分を指定しての生成
	Vector3(XMFLOAT3 o) :x(o.x), y(o.y), z(o.z) {};	// Vector3

	// メンバ関数
	float Length() const;				// ノルム(長さ)を求める
	Vector3& Normalize();				// 正規化
	float Dot(const Vector3& v) const;	// 内積を求める
	Vector3 Cross(const Vector3& v) const;// 外積を求める

	// 単項演算子オーバーロード
	Vector3 operator+() const;
	Vector3 operator-() const;
	Vector3 operator*(const Vector3& v);
	Vector3 operator/(const Vector3& v);

	// 代入演算子オーバーロード
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(const Vector3& v);
	Vector3& operator/=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);

	Vector3& operator=(float num);

	operator XMFLOAT3() const {
		return { x,y,z };
	};
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応するため、以下のように準備している
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);

// 度数法→ラジアン 変換
float Degree2Radian(float degree);

// ラジアン→度数法 変換
float Radian2Degree(float radian);
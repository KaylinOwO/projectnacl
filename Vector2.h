#include "SDK.h"
#pragma once

#ifndef VECTOR2_DEF
#define VECTOR2_DEF

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

class Vector2
{
public:
	float x, y;
	Vector2(void);
	Vector2(float X, float Y);
	void Init(float ix = 0.0f, float iy = 0.0f);
	bool IsValid() const;
	float operator[](int i) const;
	float& operator[](int i);
	inline void Zero();
	bool operator==(const Vector2& v) const;
	bool operator!=(const Vector2& v) const;
	__forceinline Vector2&	operator+=(const Vector2 &v);
	__forceinline Vector2&	operator-=(const Vector2 &v);
	__forceinline Vector2&	operator*=(const Vector2 &v);
	__forceinline Vector2&	operator*=(float s);
	__forceinline Vector2&	operator/=(const Vector2 &v);
	__forceinline Vector2&	operator/=(float s);
	__forceinline Vector2&	operator+=(float fl);
	__forceinline Vector2&	operator-=(float fl);
	inline float	Length() const;
	__forceinline float LengthSqr(void) const
	{
		CHECK_VALID(*this);
		return (x*x + y * y);
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
	}
	float	NormalizeInPlace();
	__forceinline float	DistToSqr(const Vector2 &vOther) const;
	float	Dot(const Vector2& vOther) const;
	float	Length2D(void) const;
	float	Length2DSqr(void) const;
	Vector2& operator=(const Vector2 &vOther);
	Vector2	operator-(void) const;
	Vector2	operator+(const Vector2& v) const;
	Vector2	operator-(const Vector2& v) const;
	Vector2	operator*(const Vector2& v) const;
	Vector2	operator/(const Vector2& v) const;
	Vector2	operator*(float fl) const;
	Vector2	operator/(float fl) const;
};
//===============================================
inline void Vector2::Init(float ix, float iy)
{
	x = ix, y = iy;
	CHECK_VALID(*this);
}
//===============================================
inline Vector2::Vector2(float X, float Y)
{
	x = X, y = Y;
	CHECK_VALID(*this);
}
//===============================================
inline Vector2::Vector2(void) { }
//===============================================
inline void Vector2::Zero()
{
	x = y = 0.0f;
}
//===============================================
inline void VectorClear(Vector2& a)
{
	a.x = a.y = 0.0f;
}
//===============================================
inline Vector2& Vector2::operator=(const Vector2 &vOther)
{
	CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y;
	return *this;
}
//===============================================
inline float& Vector2::operator[](int i)
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}
//===============================================
inline float Vector2::operator[](int i) const
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}
//===============================================
inline bool Vector2::operator==(const Vector2& src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y);
}
//===============================================
inline bool Vector2::operator!=(const Vector2& src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y);
}
//===============================================
__forceinline void VectorCopy(const Vector2& src, Vector2& dst)
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
}
//===============================================
__forceinline  Vector2& Vector2::operator+=(const Vector2& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x += v.x; y += v.y;
	return *this;
}
//===============================================
__forceinline  Vector2& Vector2::operator-=(const Vector2& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x -= v.x; y -= v.y;
	return *this;
}
//===============================================
__forceinline  Vector2& Vector2::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector2& Vector2::operator*=(const Vector2& v)
{
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline Vector2&	Vector2::operator+=(float fl)
{
	x += fl;
	y += fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline Vector2&	Vector2::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector2& Vector2::operator/=(float fl)
{
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector2& Vector2::operator/=(const Vector2& v)
{
	CHECK_VALID(v);
	Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
	x /= v.x;
	y /= v.y;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline float Vector2::Length(void) const
{
	CHECK_VALID(*this);

	float root = 0.0f;

	float sqsr = x * x + y * y;

	__asm
	{
		sqrtss xmm0, sqsr
		movss root, xmm0
	}

	return root;
}
//===============================================
inline float Vector2::Length2D(void) const
{
	CHECK_VALID(*this);

	float root = 0.0f;

	float sqst = x * x + y * y;

	__asm
	{
		sqrtss xmm0, sqst
		movss root, xmm0
	}

	return root;
}
//===============================================
inline float Vector2::Length2DSqr(void) const
{
	return (x*x + y * y);
}
//===============================================
float Vector2::DistToSqr(const Vector2 &vOther) const
{
	Vector2 delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;

	return delta.LengthSqr();
}
//===============================================
inline float Vector2::NormalizeInPlace()
{
	Vector2& v = *this;
	float l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f;
	}
	return l;
}
//===============================================
inline Vector2 Vector2::operator+(const Vector2& v) const
{
	Vector2 res;
	res.x = x + v.x;
	res.y = y + v.y;
	return res;
}
//===============================================
inline Vector2 Vector2::operator-(const Vector2& v) const
{
	Vector2 res;
	res.x = x - v.x;
	res.y = y - v.y;
	return res;
}
//===============================================
inline Vector2 Vector2::operator*(float fl) const
{
	Vector2 res;
	res.x = x * fl;
	res.y = y * fl;
	return res;
}
//===============================================
inline Vector2 Vector2::operator*(const Vector2& v) const
{
	Vector2 res;
	res.x = x * v.x;
	res.y = y * v.y;
	return res;
}
//===============================================
inline Vector2 Vector2::operator/(float fl) const
{
	Vector2 res;
	res.x = x / fl;
	res.y = y / fl;
	return res;
}
//===============================================
inline Vector2 Vector2::operator/(const Vector2& v) const
{
	Vector2 res;
	res.x = x / v.x;
	res.y = y / v.y;
	return res;
}
inline float Vector2::Dot(const Vector2& vOther) const
{
	const Vector2& a = *this;

	return(a.x*vOther.x + a.y*vOther.y);
}

inline Vector2 rotate_vec2(Vector2& point, const Vector2& origin, float radians)
{
	float s = sin(radians);
	float c = cos(radians);

	// translate point back to origin:
	point.x -= origin.x;
	point.y -= origin.y;

	// rotate point
	float xnew = point.x * c - point.y * s;
	float ynew = point.x * s + point.y * c;

	// translate point back:
	point.x = xnew + origin.x;
	point.y = ynew + origin.y;
	return point;
}
#endif
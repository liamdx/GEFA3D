/*
* Copyright (c) 2016-2016 Irlan Robson http://www.irlan.net
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B3_DENSE_VEC_3_H
#define B3_DENSE_VEC_3_H

#include <bounce/common/math/vec3.h>

struct b3DenseVec3
{
	b3DenseVec3()
	{
		n = 0;
		v = nullptr;
	}

	b3DenseVec3(u32 _n)
	{
		n = _n;
		v = (b3Vec3*)b3Alloc(n * sizeof(b3Vec3));
	}

	b3DenseVec3(const b3DenseVec3& _v)
	{
		n = _v.n;
		v = (b3Vec3*)b3Alloc(n * sizeof(b3Vec3));

		Copy(_v);
	}

	~b3DenseVec3()
	{
		b3Free(v);
	}

	const b3Vec3& operator[](u32 i) const
	{
		B3_ASSERT(i < n);
		return v[i];
	}

	b3Vec3& operator[](u32 i)
	{
		B3_ASSERT(i < n);
		return v[i];
	}

	b3DenseVec3& operator=(const b3DenseVec3& _v)
	{
		if (_v.v == v)
		{
			return *this;
		}
		
		if (n == _v.n)
		{
			Copy(_v);
			return *this;
		}

		b3Free(v);

		n = _v.n;
		v = (b3Vec3*)b3Alloc(n * sizeof(b3Vec3));
		
		Copy(_v);

		return *this;
	}

	void Copy(const b3DenseVec3& _v)
	{
		B3_ASSERT(n == _v.n);
		memcpy(v, _v.v, n * sizeof(b3Vec3));
	}

	void SetZero()
	{
		for (u32 i = 0; i < n; ++i)
		{
			v[i].SetZero();
		}
	}

	b3Vec3* v;
	u32 n;
};

inline void b3Add(b3DenseVec3& out, const b3DenseVec3& a, const b3DenseVec3& b)
{
	B3_ASSERT(out.n == a.n && a.n == b.n);

	for (u32 i = 0; i < a.n; ++i)
	{
		out[i] = a[i] + b[i];
	}
}

inline void b3Sub(b3DenseVec3& out, const b3DenseVec3& a, const b3DenseVec3& b)
{
	B3_ASSERT(out.n == a.n && a.n == b.n);

	for (u32 i = 0; i < a.n; ++i)
	{
		out[i] = a[i] - b[i];
	}
}

inline void b3Mul(b3DenseVec3& out, float32 a, const b3DenseVec3& b)
{
	B3_ASSERT(out.n == b.n);

	for (u32 i = 0; i < b.n; ++i)
	{
		out[i] = a * b[i];
	}
}

inline void b3Negate(b3DenseVec3& out, const b3DenseVec3& v)
{
	b3Mul(out, -1.0f, v);
}

inline float32 b3Dot(const b3DenseVec3& a, const b3DenseVec3& b)
{
	B3_ASSERT(a.n == b.n);

	float32 result = 0.0f;

	for (u32 i = 0; i < a.n; ++i)
	{
		result += b3Dot(a[i], b[i]);
	}
	
	return result;
}

inline b3DenseVec3 operator+(const b3DenseVec3& a, const b3DenseVec3& b)
{
	b3DenseVec3 result(a.n);
	b3Add(result, a, b);
	return result;
}

inline b3DenseVec3 operator-(const b3DenseVec3& a, const b3DenseVec3& b)
{
	b3DenseVec3 result(a.n);
	b3Sub(result, a, b);
	return result;
}

inline b3DenseVec3 operator*(float32 a, const b3DenseVec3& b)
{
	b3DenseVec3 result(b.n);
	b3Mul(result, a, b);
	return result;
}

#endif
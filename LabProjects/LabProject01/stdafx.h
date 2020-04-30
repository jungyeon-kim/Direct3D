// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
// for Direct3D
#include <DirectXMath.h> 
#include <DirectXPackedVector.h> 
#include <DirectXColors.h> 
#include <DirectXCollision.h>

#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace DirectX;
using namespace DirectX::PackedVector;

#define FRAMEBUFFER_WIDTH		640
#define FRAMEBUFFER_HEIGHT		480

#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))

#define DegreeToRadian(x)		float((x)*3.141592654f/180.0f)

#define EPSILON					1.0e-2f

inline bool IsZero(float fValue) { return((fabsf(fValue) <= EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

namespace Vector3
{
	inline XMFLOAT3 Add(const XMFLOAT3& Left, const XMFLOAT3& Right)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVectorAdd(XMLoadFloat3(&Left), XMLoadFloat3(&Right)));

		return(Result);
	}
	inline XMFLOAT3 Subtract(const XMFLOAT3& Left, const XMFLOAT3& Right)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVectorSubtract(XMLoadFloat3(&Left), XMLoadFloat3(&Right)));
		return(Result);
	}
	inline XMFLOAT3 Multiply(const XMFLOAT3& Left, const XMFLOAT3& Right)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVectorMultiply(XMLoadFloat3(&Left), XMLoadFloat3(&Right)));
		return(Result);
	}
	inline XMFLOAT3 Divide(const XMFLOAT3& Left, const XMFLOAT3& Right)
	{
		XMFLOAT3 Result;
		XMStoreFloat3(&Result, XMVectorDivide(XMLoadFloat3(&Left), XMLoadFloat3(&Right)));
		return(Result);
	}
}

namespace Matrix3x3
{
	inline XMFLOAT3X3 Add(const XMFLOAT3X3& Left, const XMFLOAT3X3& Right)
	{
		XMFLOAT3X3 Result;
		XMStoreFloat3x3(&Result, XMLoadFloat3x3(&Left) + XMLoadFloat3x3(&Right));
		return(Result);
	}
	inline XMFLOAT3X3 Subtract(const XMFLOAT3X3& Left, const XMFLOAT3X3& Right)
	{
		XMFLOAT3X3 Result;
		XMStoreFloat3x3(&Result, XMLoadFloat3x3(&Left) - XMLoadFloat3x3(&Right));
		return(Result);
	}
	inline XMFLOAT3X3 Multiply(const XMFLOAT3X3& Left, const XMFLOAT3X3& Right)
	{
		XMFLOAT3X3 Result;
		XMStoreFloat3x3(&Result, XMLoadFloat3x3(&Left) * XMLoadFloat3x3(&Right));
		return(Result);
	}
}
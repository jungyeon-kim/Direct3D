// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

// for Direct3D
#include <DirectXMath.h> 
#include <DirectXPackedVector.h> 
#include <DirectXColors.h> 
#include <DirectXCollision.h>
using namespace DirectX; 
using namespace DirectX::PackedVector;

#define DIR_FORWARD 0x01 
#define DIR_BACKWARD 0x02 
#define DIR_LEFT 0x04 
#define DIR_RIGHT 0x08 
#define DIR_UP 0x10 
#define DIR_DOWN 0x20

#define FRAMEBUFFER_WIDTH		640
#define FRAMEBUFFER_HEIGHT		480

#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))

#define DegreeToRadian(x)		float((x)*3.141592654f/180.0f)

#define EPSILON					1.0e-2f

inline bool IsZero(float fValue) { return((fabsf(fValue) <= EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

namespace Matrix4x4 
{
	inline XMFLOAT4X4 Identity() 
	{ 
		XMFLOAT4X4 xmmtx4x4Result; 
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity()); 
		return(xmmtx4x4Result); 
	}
}

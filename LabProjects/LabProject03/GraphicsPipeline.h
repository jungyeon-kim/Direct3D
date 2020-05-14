#pragma once

#include "Camera.h"

class CGraphicsPipeline
{
private:
	//현재 렌더링할 게임 객체의 월드 변환 행렬이다. 
	static XMFLOAT4X4* m_pxmf4x4World;
	//현재 카메라 객체의 (카메라 변환 행렬 * 원근 투영 변환 행렬)이다. 
	static XMFLOAT4X4* m_pxmf4x4ViewProject;
	static CViewport* m_pViewport;
public:
	static void SetWorldTransform(XMFLOAT4X4* pxmf4x4World) { m_pxmf4x4World = pxmf4x4World; }
	static void SetViewProjectTransform(XMFLOAT4X4* pxmf4x4ViewProject) { m_pxmf4x4ViewProject = pxmf4x4ViewProject; }
	static void SetViewport(CViewport* pViewport) { m_pViewport = pViewport; }
	static XMFLOAT3 ScreenTransform(XMFLOAT3& xmf3Project); 
	static XMFLOAT3 Project(XMFLOAT3& xmf3Model);
};


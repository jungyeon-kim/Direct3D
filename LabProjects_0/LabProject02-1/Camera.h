#pragma once

class CPoint3D;

class CViewport 
{
public: 
	CViewport(int nLeft, int nTop, int nWidth, int nHeight) 
	{ 
		m_nLeft = nLeft; 
		m_nTop = nTop; 
		m_nWidth = nWidth; 
		m_nHeight = nHeight; 
	} 
	virtual ~CViewport() {}
	
	int m_nLeft; 
	int m_nTop; 
	int m_nWidth; 
	int m_nHeight;
};

class CCamera
{
public:
	CCamera() {}
	virtual ~CCamera() { if (m_pViewport) { delete m_pViewport; m_pViewport = nullptr; } }
private:
	//카메라의 위치(월드 좌표계) 
	float m_fxPosition = 0.0f, m_fyPosition = 0.0f, m_fzPosition = 0.0f;
	//카메라의 회전(카메라 좌표계) 
	float m_fxRotation = 0.0f, m_fyRotation = 0.0f, m_fzRotation = 0.0f;
	//카메라의 시야각, 투영 사각형까지의 거리 
	float m_fFOVAngle = 90.0f, m_fProjectRectDistance = 1.0f;
	//뷰포트 
	CViewport* m_pViewport = nullptr;
	//뷰포트의 가로 길이와 세로 길이의 비율(종횡비: Aspect ratio) 
	float m_fAspectRatio = float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT);
public:
	//카메라 변환, 투영 변환, 화면 변환을 수행하는 한다. 
	CPoint3D CameraTransform(CPoint3D& f3World);
	CPoint3D ProjectionTransform(CPoint3D& f3Camera);
	CPoint3D ScreenTransform(CPoint3D& f3Projection);
	void SetPosition(float x, float y, float z) { m_fxPosition = x; m_fyPosition = y; m_fzPosition = z; }
	void SetRotation(float fPitch, float fYaw, float fRoll) { m_fxRotation = fPitch; m_fyRotation = fYaw; m_fzRotation = fRoll; }
	//카메라의 뷰포트와 시야각을 설정한다. 
	void SetViewport(int xStart, int yStart, int nWidth, int nHeight);
	void SetFOVAngle(float fFOVAngle);
	//카메라를 이동하고 회전한다. 
	void Move(float x, float y, float z);
	void Rotate(float fPitch, float fYaw, float fRoll);
};
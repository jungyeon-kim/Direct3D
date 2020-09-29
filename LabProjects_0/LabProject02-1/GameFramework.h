#pragma once

class CGameFramework
{
public: 
	CGameFramework() {} 
	~CGameFramework() {}
private: 
	//������ �������α׷��� �ν��Ͻ� �ڵ�� �� ������ �ڵ��̴�. 
	HINSTANCE m_hInstance = NULL; 
	HWND m_hWnd = NULL;
	//�� �������� Ŭ���̾�Ʈ �簢�� �����̴�. 
	RECT m_rcClient{};
	//�������� ����� �Ǵ� ȭ�鿡 �ش��ϴ� ��Ʈ�ʰ� ��Ʈ�� ����̽� ���ؽ�Ʈ(Device Context)�̴�. 
	HDC m_hDCFrameBuffer = NULL; 
	HBITMAP m_hBitmapFrameBuffer = NULL; 
	HBITMAP m_hBitmapSelect = NULL;
	//�÷��̾� ��ü�̴�. 
	class CPlayer* m_pPlayer = nullptr; 
	//���� ��ü���� �����ϴ� ��(���� ����) Ŭ�����̴�. 
	class CScene* m_pScene = nullptr;
public: 
	//�����ӿ�ũ�� �����ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�). 
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd); 
	//�����ӿ�ũ�� �Ҹ��ϴ� �Լ��̴�(�������α׷��� ����Ǹ� ȣ��ȴ�). 
	void OnDestroy();
	//���� ���踦 �������� ��Ʈ�� ǥ���� �����ϰ�, �����, Ŭ���̾�Ʈ �������� �����Ѵ�. 
	void BuildFrameBuffer(); 
	void ClearFrameBuffer(DWORD dwColor); 
	void PresentFrameBuffer();
	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
	void BuildObjects(); 
	void ReleaseObjects();
	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput(); 
	void AnimateObjects();
	void FrameAdvance();
};


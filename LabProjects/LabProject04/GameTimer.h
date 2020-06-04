#pragma once

constexpr ULONG MAX_SAMPLE_COUNT = 50; // 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CGameTimer
{
private:
	// ��ǻ�Ͱ� Performance Counter�� ������ �ִ� ��
	bool m_bHardwareHasPerformanceCounter;
	// Scale Counter�� ��
	float m_fTimeScale;
	// ������ ������ ���� ������ �ð�
	float m_fTimeElapsed;
	// ���� �ð�
	__int64 m_nCurrentTime;
	// ������ �������� �ð�
	__int64 m_nLastTime;
	// ��ǻ���� Performance Frequency
	__int64 m_nPerformanceFrequency;

	// ������ �ð��� �����ϱ� ���� �迭
	float m_fFrameTime[MAX_SAMPLE_COUNT];
	// ������ ������ Ƚ��
	ULONG m_nSampleCount;

	// ������ ������ ����Ʈ
	unsigned long m_nCurrentFrameRate;
	// �ʴ� ������ ��
	unsigned long m_nFramesPerSecond;
	// ������ ����Ʈ ��� �ҿ� �ð�
	float m_fFPSTimeElapsed;
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float fLockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR lpszString = nullptr, int nCharacters = 0);
	float GetTimeElapsed();
};


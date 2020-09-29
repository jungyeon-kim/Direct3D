#pragma once

constexpr ULONG MAX_SAMPLE_COUNT = 50; // 50회의 프레임 처리시간을 누적하여 평균한다.

class CGameTimer
{
private:
	// 컴퓨터가 Performance Counter를 가지고 있는 가
	bool m_bHardwareHasPerformanceCounter;
	// Scale Counter의 양
	float m_fTimeScale;
	// 마지막 프레임 이후 지나간 시간
	float m_fTimeElapsed;
	// 현재 시간
	__int64 m_nCurrentTime;
	// 마지막 프레임의 시간
	__int64 m_nLastTime;
	// 컴퓨터의 Performance Frequency
	__int64 m_nPerformanceFrequency;

	// 프레임 시간을 누적하기 위한 배열
	float m_fFrameTime[MAX_SAMPLE_COUNT];
	// 누적된 프레임 횟수
	ULONG m_nSampleCount;

	// 현재의 프레임 레이트
	unsigned long m_nCurrentFrameRate;
	// 초당 프레임 수
	unsigned long m_nFramesPerSecond;
	// 프레임 레이트 계산 소요 시간
	float m_fFPSTimeElapsed;
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float fLockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR lpszString = nullptr, int nCharacters = 0);
	float GetTimeElapsed();
};


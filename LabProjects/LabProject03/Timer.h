#pragma once
//-----------------------------------------------------------------------------
// File: CGameTimer.h
//-----------------------------------------------------------------------------

const ULONG MAX_SAMPLE_COUNT = 50; // Maximum frame time sample count

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();
	//타이머의 시간을 갱신한다. 
	void Tick(float fLockFPS = 0.0f);
	//프레임 레이트를 문자열로 반환한다. 
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	//현재 프레임의 경과 시간을 반환한다.
	float GetTimeElapsed();
private:
	double m_fTimeScale{};
	float m_fTimeElapsed{};
	__int64 m_nBasePerformanceCounter{};
	__int64 m_nPausedPerformanceCounter{};
	__int64 m_nStopPerformanceCounter{};
	__int64 m_nCurrentPerformanceCounter{};
	__int64 m_nLastPerformanceCounter{};
	__int64 m_PerformanceFrequencyPerSec{};
	float m_fFrameTime[MAX_SAMPLE_COUNT]{};
	ULONG m_nSampleCount{};
	unsigned long m_nCurrentFrameRate{};
	unsigned long m_FramePerSecond{};
	float m_fFPSTimeElapsed{};
};
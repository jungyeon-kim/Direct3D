#include "stdafx.h"
#include "GameTimer.h"

CGameTimer::CGameTimer()
{
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&PerformanceFrequency))
	{
		bIsHardwareHasPerformanceCounter = TRUE;
		QueryPerformanceCounter((LARGE_INTEGER*)&LastTime);
		TimeScale = 1.0f / PerformanceFrequency;
	}
	else
	{
		bIsHardwareHasPerformanceCounter = FALSE;
		LastTime = timeGetTime();
		TimeScale = 0.001f;
	}
}
CGameTimer::~CGameTimer()
{
}

void CGameTimer::Tick(float LockFPS)
{
	if (bIsHardwareHasPerformanceCounter) QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	else CurrentTime = timeGetTime();

	// 마지막으로 이 함수를 호출한 이후 경과한 시간을 계산
	float fTimeElapsed{ (CurrentTime - LastTime) * TimeScale };
	if (LockFPS > 0.0f)
	{
		// LockFPS가 0보다 크면 이 시간만큼 호출한 함수를 기다림
		while (fTimeElapsed < 1.0f / LockFPS)
		{
			if (bIsHardwareHasPerformanceCounter) QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
			else CurrentTime = ::timeGetTime();
			// 마지막으로 이 함수를 호출한 이후 경과한 시간을 계산
			fTimeElapsed = (CurrentTime - LastTime) * TimeScale;
		}
	}

	// 현재 시간을 LastTime에 Set
	LastTime = CurrentTime;

	/* 마지막 프레임 처리 시간과 현재 프레임 처리 시간의 차이가 1초보다 작으면 현재 프레임 처리 시간
	을 FrameTime[0]에 Set */
	if (fabsf(fTimeElapsed - ElapsedTime) < 1.0f)
	{
		memmove(&FrameTime[1], FrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		FrameTime[0] = fTimeElapsed;
		if (SampleCount < MAX_SAMPLE_COUNT) SampleCount++;
	}

	// 초당 프레임 수를 1 증가시키고 현재 프레임 처리 시간을 누적하여 Set
	++FramesPerSecond;
	FPSElapsedTime += fTimeElapsed;
	if (FPSElapsedTime > 1.0f)
	{
		CurrentFrameRate = FramesPerSecond;
		FramesPerSecond = 0;
		FPSElapsedTime = 0.0f;
	}

	//누적된 프레임 처리 시간의 평균을 구하여 프레임 처리 시간을 구함
	ElapsedTime = 0.0f;
	for (ULONG i = 0; i < SampleCount; ++i) ElapsedTime += FrameTime[i];
	if (SampleCount > 0) ElapsedTime /= SampleCount;
}

unsigned long CGameTimer::GetFrameRate(LPTSTR String, int Size)
{
	//현재 프레임 레이트를 문자열로 변환하여 lpszString 버퍼에 쓰고 "FPS"와 결합한다. 
	if (String)
	{
		_itow_s(CurrentFrameRate, String, Size, 10);
		wcscat_s(String, Size, _T(" FPS)"));
	}

	return CurrentFrameRate;
}

float CGameTimer::GetElapsedTime()
{
	return ElapsedTime;
}
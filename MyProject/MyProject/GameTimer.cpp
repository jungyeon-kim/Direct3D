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

	// ���������� �� �Լ��� ȣ���� ���� ����� �ð��� ���
	float fTimeElapsed{ (CurrentTime - LastTime) * TimeScale };
	if (LockFPS > 0.0f)
	{
		// LockFPS�� 0���� ũ�� �� �ð���ŭ ȣ���� �Լ��� ��ٸ�
		while (fTimeElapsed < 1.0f / LockFPS)
		{
			if (bIsHardwareHasPerformanceCounter) QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
			else CurrentTime = ::timeGetTime();
			// ���������� �� �Լ��� ȣ���� ���� ����� �ð��� ���
			fTimeElapsed = (CurrentTime - LastTime) * TimeScale;
		}
	}

	// ���� �ð��� LastTime�� Set
	LastTime = CurrentTime;

	/* ������ ������ ó�� �ð��� ���� ������ ó�� �ð��� ���̰� 1�ʺ��� ������ ���� ������ ó�� �ð�
	�� FrameTime[0]�� Set */
	if (fabsf(fTimeElapsed - ElapsedTime) < 1.0f)
	{
		memmove(&FrameTime[1], FrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		FrameTime[0] = fTimeElapsed;
		if (SampleCount < MAX_SAMPLE_COUNT) SampleCount++;
	}

	// �ʴ� ������ ���� 1 ������Ű�� ���� ������ ó�� �ð��� �����Ͽ� Set
	++FramesPerSecond;
	FPSElapsedTime += fTimeElapsed;
	if (FPSElapsedTime > 1.0f)
	{
		CurrentFrameRate = FramesPerSecond;
		FramesPerSecond = 0;
		FPSElapsedTime = 0.0f;
	}

	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ����
	ElapsedTime = 0.0f;
	for (ULONG i = 0; i < SampleCount; ++i) ElapsedTime += FrameTime[i];
	if (SampleCount > 0) ElapsedTime /= SampleCount;
}

unsigned long CGameTimer::GetFrameRate(LPTSTR String, int Size)
{
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� "FPS"�� �����Ѵ�. 
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
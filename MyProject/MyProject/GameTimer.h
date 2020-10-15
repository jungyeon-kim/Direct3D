#pragma once

constexpr ULONG MAX_SAMPLE_COUNT{ 50 }; // 50회의 프레임 처리시간을 누적하여 평균한다.

class CGameTimer
{
private:
	// 컴퓨터가 Performance Counter를 가지고 있는 가
	bool bIsHardwareHasPerformanceCounter{};
	// Scale Counter의 양
	float TimeScale{};
	// 마지막 프레임 이후 지나간 시간
	float ElapsedTime{};
	// 현재 시간
	__int64 CurrentTime{};
	// 마지막 프레임의 시간
	__int64 LastTime{};
	// 컴퓨터의 Performance Frequency
	__int64 PerformanceFrequency{};

	// 프레임 시간을 누적하기 위한 배열
	float FrameTime[MAX_SAMPLE_COUNT]{};
	// 누적된 프레임 횟수
	ULONG SampleCount{};

	// 현재의 프레임 레이트
	unsigned long CurrentFrameRate{};
	// 초당 프레임 수
	unsigned long FramesPerSecond{};
	// 프레임 레이트 계산 소요 시간
	float FPSElapsedTime{};
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float LockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR String = nullptr, int Size = 0);
	float GetElapsedTime();
};


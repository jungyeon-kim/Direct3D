#pragma once

constexpr ULONG MAX_SAMPLE_COUNT{ 50 }; // 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CGameTimer
{
private:
	// ��ǻ�Ͱ� Performance Counter�� ������ �ִ� ��
	bool bIsHardwareHasPerformanceCounter{};
	// Scale Counter�� ��
	float TimeScale{};
	// ������ ������ ���� ������ �ð�
	float ElapsedTime{};
	// ���� �ð�
	__int64 CurrentTime{};
	// ������ �������� �ð�
	__int64 LastTime{};
	// ��ǻ���� Performance Frequency
	__int64 PerformanceFrequency{};

	// ������ �ð��� �����ϱ� ���� �迭
	float FrameTime[MAX_SAMPLE_COUNT]{};
	// ������ ������ Ƚ��
	ULONG SampleCount{};

	// ������ ������ ����Ʈ
	unsigned long CurrentFrameRate{};
	// �ʴ� ������ ��
	unsigned long FramesPerSecond{};
	// ������ ����Ʈ ��� �ҿ� �ð�
	float FPSElapsedTime{};
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float LockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR String = nullptr, int Size = 0);
	float GetElapsedTime();
};


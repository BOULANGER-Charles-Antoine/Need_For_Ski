#pragma once
#include <memory>
#include "Manager.h"
#include "Horloge.h"

class Time : public Manager<Time>
{
	std::unique_ptr<Horloge> horloge = nullptr;

	int64_t lastUpdateTime;
	double deltaTime;

public:
	bool Init(Horloge* horloge) noexcept;
	void Start() override;
	void Update() override;
	void Close() noexcept override;

	static double GetTimeSinceLastUpdate();
	static double GetDeltaTime() noexcept;
	static const Horloge& GetHorloge() noexcept;
	static double GetFixedDeltaTime() noexcept;
};

class Timer
{
	bool isRunning = false;

	double duration = 0.0;
	double time = 0.0;

public:
	Timer() = default;

	void Start(double duration) noexcept
	{
		isRunning = true;

		this->duration = duration;
		time = 0.0;
	}

	void Stop() noexcept
	{
		isRunning = false;
		time = 0.0;
	}

	void Update() noexcept
	{
		time += Time::GetDeltaTime();
	}

	bool IsRunning() const noexcept { return isRunning; }
	bool Done() const noexcept { return time >= duration && isRunning; }
	double GetDuration() const { return duration; }
	double GetTime() const { return time; }
};
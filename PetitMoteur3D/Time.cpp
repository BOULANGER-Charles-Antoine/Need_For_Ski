#include "stdafx.h"
#include "Time.h"

using namespace std;

bool Time::Init(Horloge* horloge) noexcept
{
    this->horloge = unique_ptr<Horloge>(horloge);
    return true;
}

void Time::Start()
{
    if (!horloge) return;

    lastUpdateTime = horloge->GetTimeCount();
}

void Time::Update()
{
    if (!horloge) return;

    const int64_t currentTime = horloge->GetTimeCount();
    deltaTime = horloge->GetTimeBetweenCounts(lastUpdateTime, currentTime);
    lastUpdateTime = currentTime;
}

void Time::Close() noexcept
{
    horloge = nullptr;

    lastUpdateTime = 0;
    deltaTime = 0;
}

double Time::GetTimeSinceLastUpdate()
{
    Time& time = Time::GetInstance();
    if (!time.horloge) return 0;

    const int64_t currentTime = time.horloge->GetTimeCount();
    return time.horloge->GetTimeBetweenCounts(time.lastUpdateTime, currentTime);
}

double Time::GetDeltaTime() noexcept
{
    return Time::GetInstance().deltaTime;
}

const Horloge& Time::GetHorloge() noexcept
{
    return *Time::GetInstance().horloge;
}

double Time::GetFixedDeltaTime() noexcept
{
    return 1.0f / 60.0f;
}

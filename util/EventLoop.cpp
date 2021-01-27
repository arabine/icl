/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include "EventLoop.h"

static std::chrono::steady_clock::time_point Next(std::chrono::milliseconds delay)
{
    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
    return tp + delay;
}

/*****************************************************************************/
EventLoop::EventLoop()
    : mStopRequested(false)
    , mWaitDelay(200)
{

}
/*****************************************************************************/
EventLoop::~EventLoop()
{
    Stop();
}
/*****************************************************************************/
void EventLoop::Stop()
{
    mAccessGuard.lock();
    mStopRequested = true;
    mAccessGuard.unlock();

    if (mThread.joinable())
    {
        mThread.join();
    }
}
/*****************************************************************************/
void EventLoop::AddTimer(const std::string &name, std::chrono::milliseconds period, CallBack callBack)
{
    Timer tm;

    tm.period = period;
    tm.callBack = callBack;
    tm.next = Next(period);

    mAccessGuard.lock();
    mTimers.insert(std::make_pair(name, tm));
    mAccessGuard.unlock();
}
/*****************************************************************************/
bool EventLoop::ModifyTimer(const std::string &name, std::chrono::milliseconds new_period)
{
    bool success = false;
    if (mTimers.count(name) > 0)
    {
        mAccessGuard.lock();
        mTimers[name].period = new_period;
        mAccessGuard.unlock();
        success = true;
    }
    return success;
}
/*****************************************************************************/
void EventLoop::Start()
{
    mThread = std::thread(&EventLoop::Loop, this);
}
/*****************************************************************************/
void EventLoop::Loop()
{
    mStopRequested = false;
    bool stop = false;

    while(!stop)
    {
        // create a time point pointing to n milliseconds in future
        std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now() + std::chrono::milliseconds(mWaitDelay);

        // Sleep Till specified time point
        // Accepts std::chrono::system_clock::time_point as argument
        std::this_thread::sleep_until(tp);

        mAccessGuard.lock();
        UpdateTimers();
        stop = mStopRequested;
        mAccessGuard.unlock();
    }
}
/*****************************************************************************/
void EventLoop::UpdateTimers()
{
    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();

    for (auto& [n, t] : mTimers)
    {
        if (tp >= t.next)
        {
            t.next = Next(t.period);
            t.callBack();
        }
    }
}

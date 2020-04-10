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
void EventLoop::AddTimer(std::chrono::milliseconds period, CallBack callBack)
{
    Timer tm;

    tm.period = period;
    tm.callBack = callBack;
    tm.next = Next(period);

    mAccessGuard.lock();
    mTimers.push_back(tm);
    mAccessGuard.unlock();
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
        // create a time point pointing to 10 second in future
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

    int i = 0;
    for (auto &t : mTimers)
    {
        if (tp >= t.next)
        {
            t.next = Next(t.period);
            t.callBack();
        }
        i++;
    }
}

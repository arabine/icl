/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef DURATION_TIMER_H
#define DURATION_TIMER_H

#include <chrono>
#include <functional>

class DurationTimer
{
public:
    DurationTimer()
        : mBegin(clock_::now())
        , mEnabled(false)
    {

    }

    uint32_t elapsed() const
    {
        return std::chrono::duration_cast<ms>(clock_::now() - mBegin).count();
    }

    void Start() {
        mBegin = clock_::now();
        mRunning = true;
    }

    void Stop() {
        mRunning = false;
    }

    void SetEnable(bool enable)
    {
        mEnabled = enable;
    }

    void Update()
    {
        if (IsElapsed() && mElapsedFunc && mEnabled)
        {
            mElapsedFunc();
        }
    }

    bool IsEnabled() const
    {
        return mEnabled;
    }

    bool IsElapsed()
    {
        return (elapsed() >= mDelay) && mRunning;
    }
    
    void SetDelay(uint32_t delay)
    {
        mDelay = delay;
    }

    void Register(std::function<void (void) > func)
    {
        mElapsedFunc = func;
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<uint32_t, std::ratio<1000> > ms;
    std::chrono::time_point<clock_> mBegin;
    bool mEnabled;
    bool mRunning;
    uint32_t mDelay;
    std::function<void (void) > mElapsedFunc;
};

#endif // DURATION_TIMER_H

//=============================================================================
// End of file DurationTimer.h
//=============================================================================

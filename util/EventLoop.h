/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <functional>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>


class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    typedef std::function<void (void)> CallBack;

    void Loop();
    void Stop();
    void Start();
    void UpdateTimers();

    // From IEventLoop
    void AddTimer(std::chrono::milliseconds period, CallBack callBack);

private:
    bool mStopRequested;
    std::thread mThread;
    std::mutex mAccessGuard;

    struct Timer
    {
        std::chrono::milliseconds period;
        CallBack callBack;
        std::chrono::steady_clock::time_point next;
    };

    std::vector<Timer> mTimers;
    uint32_t mWaitDelay;
};

#endif // EVENT_LOOP_H

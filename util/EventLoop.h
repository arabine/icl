/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <functional>
#include <map>
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

    void AddTimer(const std::string &name, std::chrono::milliseconds period, CallBack callBack);
    bool ModifyTimer(const std::string &name, std::chrono::milliseconds new_period);
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

    // name, value
    std::map<std::string, Timer> mTimers;
    uint32_t mWaitDelay;
};

#endif // EVENT_LOOP_H

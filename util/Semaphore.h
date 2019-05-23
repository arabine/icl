/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>
#include <chrono>

class Semaphore
{
public:
    Semaphore (std::uint32_t count = 0U)
        : mCount(count)
    {

    }

    inline void Notify()
    {
        mMutex.lock();
        mCount++;
        mMutex.unlock();
        mCondVar.notify_one();
    }

    inline bool Wait(int ms = 0)
    {
        bool normal = true;
        if (ms < 100)
        {
            ms = 100;
        }
        std::unique_lock<std::mutex> lock(mMutex);

        while(mCount == 0U)
        {
            normal = (mCondVar.wait_for(lock, std::chrono::milliseconds(ms)) == std::cv_status::no_timeout);

            if (!normal)
            {
                break;
            }
        }
        if (mCount > 0U)
        {
            mCount--;
        }
        return normal;
    }

private:
    std::mutex mMutex;
    std::condition_variable mCondVar;
    std::uint32_t mCount;
};

#endif // SEMAPHORE_H

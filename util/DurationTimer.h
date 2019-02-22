#ifndef DURATION_TIMER_H
#define DURATION_TIMER_H

#include <chrono>

class DurationTimer
{
public:
    DurationTimer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const {
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};

#endif // DURATION_TIMER_H

//=============================================================================
// End of file DurationTimer.h
//=============================================================================

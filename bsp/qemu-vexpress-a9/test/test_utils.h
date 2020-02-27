#pragma once

#include <utest.h>
#include <atomic>
#include <ctime>
#include <chrono>

extern std::atomic_bool done;

class G
{
    int alive_;
    bool done_;
public:
    static int n_alive;
    static bool op_run;

    G() : alive_(1), done_(false) {++n_alive;}
    G(const G& g) : alive_(g.alive_), done_(g.done_) {++n_alive;}
    ~G() 
    {
        alive_ = 0;
        --n_alive;
        if (done_)
            done = true;

        rt_kprintf("G::n_alive = %d\n", G::n_alive);
        rt_kprintf("G::op_run = %d\n", G::op_run);
    }

    void operator()()
    {
        uassert_true(alive_ == 1);
        uassert_true(n_alive >= 1);
        op_run = true;
        done_ = true;

        rt_kprintf("We are running G\n");
    }

    void operator()(int i, double j)
    {
        uassert_true(alive_ == 1);
        uassert_true(n_alive >= 1);
        uassert_true(i == 5);
        uassert_true(j == 5.5);
        op_run = true;
        done_ = true;

        rt_kprintf("We are running G with arguments\n");
    }
};

typedef class std::ratio<RT_TICK_PER_SECOND> rt_ratio;
typedef class std::chrono::duration<int64_t, rt_ratio> rt_tick_duration;

class processor_timer
{
    private:
        clock_t start_point;
        clock_t end_point;
    public:
        processor_timer() : start_point(-1), end_point(-1)
        {}
        ~processor_timer() = default;

        void start()
        {
            start_point = clock();
        }

        void stop()
        {
            end_point = clock();
        }

        clock_t tick_elapsed() const
        {
            if (start_point > 0 && end_point > start_point)
                return (end_point - start_point);
            else 
                return 0;
        }

        double sec_elapsed() const
        {
            clock_t diff = tick_elapsed();
            if (diff == 0)
                return 0;
            return static_cast<double>(diff) / RT_TICK_PER_SECOND;
        }

        long int milli_elapsed() const
        {
            double sec = sec_elapsed();
            long int milli_sec = static_cast<long int>(sec * 1000);
            
            return milli_sec;
        }
};
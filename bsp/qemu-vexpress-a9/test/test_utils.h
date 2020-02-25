#pragma once

#include <utest.h>
#include <atomic>

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



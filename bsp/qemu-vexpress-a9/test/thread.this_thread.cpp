#include <rtthread.h>
#include <utest.h>
#include <thread>
#include <cmath>

#include "test_utils.h"

static rt_err_t utest_test_init(void)
{
    G::n_alive = 0;
    G::op_run = false;
    return RT_EOK;
}

static  rt_err_t utest_test_cleanup(void)
{
    return RT_EOK;
}

static void foo()
{
    std::thread::id id = std::this_thread::get_id();
    uassert_true(id != std::thread::id());
}

static void test_get_id()
{
    std::thread thr(foo);
    thr.join();
}

static void test_yield()
{
    std::this_thread::yield();
    uassert_true(true);
}

static void test_sleep_until()
{
    std::chrono::milliseconds ms(500);
    processor_timer t;
    t.start();

    std::this_thread::sleep_until(std::chrono::steady_clock::now() + ms);

    t.stop();

    double err = t.sec_elapsed() - 0.5;

    double margin = ((double) ms.count() * 5 / 100) / 1000;

    // The time slept is within 5% of 500ms
    uassert_true(std::abs(err) < margin);
}



static void test_thread_this_thread()
{
    UTEST_UNIT_RUN(test_get_id);
    UTEST_UNIT_RUN(test_yield);
    UTEST_UNIT_RUN(test_sleep_until);
}
UTEST_TC_EXPORT(test_thread_this_thread, "components.cplusplus.thread.this_thread", 
                utest_test_init, utest_test_cleanup, 2);
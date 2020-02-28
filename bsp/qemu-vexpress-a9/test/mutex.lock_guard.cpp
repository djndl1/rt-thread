#include <rtthread.h>
#include <utest.h>
#include <thread>
#include <mutex>
#include <cstdio>
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
static std::mutex m;

static void f()
{
    processor_timer t;
    t.start();
    {
        std::lock_guard<std::mutex> lg(m);
        t.stop();
    }
    auto d = t.milli_elapsed() - 250;
    uassert_true(d < 10);
}

static void test_lock_mutex()
{
    launch_thread_when_locking(m, 250, f);
}

static void test_mutex_type()
{
    static_assert((std::is_same<std::lock_guard<std::mutex>::mutex_type,
                   std::mutex>::value), "");
}

static void f2()
{
    processor_timer t;
    t.start();
    {
        m.lock();
        std::lock_guard<std::mutex> lg(m, std::adopt_lock);
        t.stop();
    }
    auto d = t.milli_elapsed() - 250;
    uassert_true(d < 10);  // within 50ms
}

static void test_adopt_lock()
{
    launch_thread_when_locking(m, 250, f2);
}

static void test_lock_guard()
{
    UTEST_UNIT_RUN(test_lock_mutex);
    UTEST_UNIT_RUN(test_mutex_type);
    UTEST_UNIT_RUN(test_adopt_lock);
}
UTEST_TC_EXPORT(test_lock_guard, "components.cplusplus.mutex.lock_guard", 
                utest_test_init, utest_test_cleanup, 2);
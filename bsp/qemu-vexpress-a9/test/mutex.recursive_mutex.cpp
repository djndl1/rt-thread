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

static void test_default_constr()
{
    std::recursive_mutex m;
}

static std::recursive_mutex m;

static void f()
{
    processor_timer t;
    t.start();
    m.lock();
    t.stop();
    m.lock();
    m.unlock();
    m.unlock();

    auto diff = std::fabs(t.sec_elapsed());
    uassert_true((diff - 0.25) < 0.1);
}

static void test_lock_unlock()
{
    m.lock();
    std::thread t(f);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    m.unlock();
    t.join();
}

static void f2()
{
    processor_timer t;
    t.start();
    uassert_true(!m.try_lock());
    uassert_true(!m.try_lock());
    uassert_true(!m.try_lock());
    while(!m.try_lock())
        ;
    t.stop();
    uassert_true(m.try_lock());
    m.unlock();
    m.unlock();

    auto diff = t.sec_elapsed();
    uassert_true((diff - 0.25) < 0.01);  // within 200ms
}

static void test_try_lock()
{
    m.lock();
    std::thread t(f2);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    m.unlock();
    t.join();
}

static void test_recursive_mutex()
{
    UTEST_UNIT_RUN(test_default_constr);
    UTEST_UNIT_RUN(test_lock_unlock);
    UTEST_UNIT_RUN(test_try_lock);
}
UTEST_TC_EXPORT(test_recursive_mutex, "components.cplusplus.mutex.recursive_mutex", 
                utest_test_init, utest_test_cleanup, 2);
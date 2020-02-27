#include <rtthread.h>
#include <utest.h>
#include <thread>
#include <mutex>
#include <cstdio>

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

static void  test_mutex_default_constr(void)
{
    static_assert(std::is_nothrow_default_constructible<std::mutex>::value, "");
    std::mutex m;
    ((void)m);
}

static std::mutex m;

static void f()
{
    processor_timer t;
    t.start();
    m.lock();
    t.stop();
    m.unlock();
    auto elapse = t.sec_elapsed();
    printf("Elapsed %f sec\n", elapse);
    uassert_true(elapse - 2.5 < 0.01);
}

static void test_lock_unlock()
{
    m.lock();
    std::thread t(f);
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
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
    m.unlock();
    auto d = ::abs((t.milli_elapsed() - 2500));
    auto elp = t.sec_elapsed();
    printf("Elapsed %f sec, %ld millisecs\n", elp, t.milli_elapsed());
    printf("Error %u millisecs\n", d);
    uassert_true(d < 100); 
}

static void test_try_lock()
{
    m.lock();
    std::thread t(f2);
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    m.unlock();
    t.join();
}

static void test_mutex()
{
    UTEST_UNIT_RUN(test_mutex_default_constr);
    UTEST_UNIT_RUN(test_lock_unlock);
    UTEST_UNIT_RUN(test_try_lock); 
}
UTEST_TC_EXPORT(test_mutex, "components.cplusplus.mutex.mutex", 
                utest_test_init, utest_test_cleanup, 2);
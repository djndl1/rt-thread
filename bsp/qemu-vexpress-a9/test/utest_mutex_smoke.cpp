#include <rtthread.h>
#include <utest.h>
#include <mutex>
#include <thread>

std::mutex m;
int foo;
static rt_err_t utest_mutex_init(void)
{
    foo = 0;
    return RT_EOK;
}

static  rt_err_t utest_mutex_cleanup(void)
{
    return RT_EOK;
}


static void lock_guard_protect()
{
    std::lock_guard<std::mutex> lock(m);
    foo++;
}

static void lock_unlock_protect()
{
    m.lock();
    foo++;
    m.unlock();
}

static void launch_threads(void (*func)(void))
{
    std::thread thr1{func};
    std::thread thr2{func};

    thr1.join();
    thr2.join();
}

static void test_lock_guard()
{
    foo = 0;
    launch_threads(lock_guard_protect);
    uassert_int_equal(foo, 2);
}

static void test_mutex_lock_unlock()
{
    foo = 0;
    launch_threads(lock_unlock_protect);
    uassert_int_equal(foo, 2);
}

static void test_mutex_smoke(void)
{
    UTEST_UNIT_RUN(test_lock_guard);
    UTEST_UNIT_RUN(test_mutex_lock_unlock);
}
UTEST_TC_EXPORT(test_mutex_smoke, "components.cplusplus.mutex.smoke", utest_mutex_init, utest_mutex_cleanup,10);
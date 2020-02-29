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
    std::unique_lock<std::mutex> lk(m, std::defer_lock);
    processor_timer t;
    t.start();
    lk.lock();
    t.stop();
    uassert_true(lk.owns_lock() == true);
    auto d = t.milli_elapsed() - 250;
    uassert_true(d < 25);  // within 25ms
    try
    {
        lk.lock();
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EDEADLK);
    }
        lk.unlock();
        lk.release();
    try
    {
        lk.lock();
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EPERM);
    }
}

static void test_lock()
{
    launch_thread_when_locking(m, 250, f);
}

static bool try_lock_called = false;
static bool try_lock_for_called = false;
static bool try_lock_until_called = false;
static bool unlock_called = false;

struct mutex
{
    bool try_lock()
    {
        try_lock_called = !try_lock_called;
        return try_lock_called;
    }
    void unlock() { unlock_called = true; }
    void lock() {}

    template <class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time)
    {
        uassert_true(rel_time == std::chrono::milliseconds(5));
        try_lock_for_called = !try_lock_for_called;
        return try_lock_for_called;
    }

    template <class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& abs_time)
    {
        typedef std::chrono::milliseconds ms;
        uassert_true(Clock::now() - abs_time < ms(5));
        try_lock_until_called = !try_lock_until_called;
        return try_lock_until_called;
    }
};

static mutex mut;

static void test_try_lock()
{
    std::unique_lock<mutex> lk(mut, std::defer_lock);
    uassert_true(lk.try_lock() == true);
    uassert_true(try_lock_called == true);
    uassert_true(lk.owns_lock() == true);
    try
    {
        lk.try_lock();
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EDEADLK);
    }
    lk.unlock();
    uassert_true(lk.try_lock() == false);
    uassert_true(try_lock_called == false);
    uassert_true(lk.owns_lock() == false);
    lk.release();
    try
    {
        lk.try_lock();
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EPERM);
    }
}

static void test_try_lock_for()
{
    std::unique_lock<mutex> lk(mut, std::defer_lock);
    uassert_true(lk.try_lock_for(std::chrono::milliseconds(5)) == true);
    uassert_true(try_lock_for_called == true);
    uassert_true(lk.owns_lock() == true);
    try
    {
        lk.try_lock_for(std::chrono::milliseconds(5));
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EDEADLK);
    }
    lk.unlock();
    uassert_true(lk.try_lock_for(std::chrono::milliseconds(5)) == false);
    uassert_true(try_lock_for_called == false);
    uassert_true(lk.owns_lock() == false);
    lk.release();
    try
    {
        lk.try_lock_for(std::chrono::milliseconds(5));
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EPERM);
    }
}

static void test_try_lock_until()
{
    std::unique_lock<mutex> lk(mut, std::defer_lock);
    uassert_true(lk.try_lock_until(std::chrono::steady_clock::now()) == true);
    uassert_true(try_lock_until_called == true);
    uassert_true(lk.owns_lock() == true);
    try
    {
        lk.try_lock_until(std::chrono::steady_clock::now());
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EDEADLK);
    }
    lk.unlock();
    uassert_true(lk.try_lock_until(std::chrono::steady_clock::now()) == false);
    uassert_true(try_lock_until_called == false);
    uassert_true(lk.owns_lock() == false);
    lk.release();
    try
    {
        lk.try_lock_until(std::chrono::steady_clock::now());
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EPERM);
    }
}

static void test_unlock()
{
    std::unique_lock<mutex> lk(mut);
    lk.unlock();
    uassert_true(unlock_called == true);
    uassert_true(lk.owns_lock() == false);
    try
    {
        lk.unlock();
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EPERM);
    }
    lk.release();
    try
    {
        lk.unlock();
        uassert_true(false);
    }
    catch (std::system_error& e)
    {
        uassert_true(e.code().value() == EPERM);
    }
}

static void test_unique_lock_locking()
{
    UTEST_UNIT_RUN(test_lock);
    UTEST_UNIT_RUN(test_try_lock);
    UTEST_UNIT_RUN(test_try_lock_for);
    UTEST_UNIT_RUN(test_try_lock_until);
    UTEST_UNIT_RUN(test_unlock);
}
UTEST_TC_EXPORT(test_unique_lock_locking, "components.cplusplus.mutex.unique_lock.locking", 
                utest_test_init, utest_test_cleanup, 2);
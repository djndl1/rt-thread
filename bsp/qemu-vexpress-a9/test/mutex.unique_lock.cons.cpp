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
    std::unique_lock<std::mutex> ul;
    uassert_true(!ul.owns_lock());
    uassert_true(ul.mutex() == nullptr);
}

static void test_move_constr()
{
    {
        typedef std::mutex M;
        M m;
        std::unique_lock<M> lk0(m);
        std::unique_lock<M> lk = std::move(lk0);
        uassert_true(lk.mutex() == std::addressof(m));
        uassert_true(lk.owns_lock() == true);
        uassert_true(lk0.mutex() == nullptr);
        uassert_true(lk0.owns_lock() == false);
    }
    {
        typedef nasty_mutex M;
        M m;
        std::unique_lock<M> lk0(m);
        std::unique_lock<M> lk = std::move(lk0);
        uassert_true(lk.mutex() == std::addressof(m));
        uassert_true(lk.owns_lock() == true);
        uassert_true(lk0.mutex() == nullptr);
        uassert_true(lk0.owns_lock() == false);
    }
}

static std::mutex mut;

static void f()
{
    processor_timer t;
    t.start();
    {
        std::unique_lock<std::mutex> lk(mut);
        t.stop();
    }
    auto d = t.milli_elapsed() - 250;
    uassert_true(d < 10);
}

static void test_mutex()
{
    launch_thread_when_locking(mut, 250, f);
}

static void test_move_assign()
{
   {
        typedef std::mutex M;
        M m0;
        M m1;
        std::unique_lock<M> lk0(m0);
        std::unique_lock<M> lk1(m1);
        lk1 = std::move(lk0);
        uassert_true(lk1.mutex() == std::addressof(m0));
        uassert_true(lk1.owns_lock() == true);
        uassert_true(lk0.mutex() == nullptr);
        uassert_true(lk0.owns_lock() == false);
    }
    {
        typedef nasty_mutex M;
        M m0;
        M m1;
        std::unique_lock<M> lk0(m0);
        std::unique_lock<M> lk1(m1);
        lk1 = std::move(lk0);
        uassert_true(lk1.mutex() == std::addressof(m0));
        uassert_true(lk1.owns_lock() == true);
        uassert_true(lk0.mutex() == nullptr);
        uassert_true(lk0.owns_lock() == false);
    }

}

static void f2()
{
    processor_timer tm;
    tm.start();
    {
        std::unique_lock<std::mutex> lk(mut, std::try_to_lock);
        uassert_true(lk.owns_lock() == false);
    }
    {
        std::unique_lock<std::mutex> lk(mut, std::try_to_lock);
        uassert_true(lk.owns_lock() == false);
    }
    {
        std::unique_lock<std::mutex> lk(mut, std::try_to_lock);
        uassert_true(lk.owns_lock() == false);
    }
    while (true)
    {
        std::unique_lock<std::mutex> lk(mut, std::try_to_lock);
        if (lk.owns_lock())
            break;
    }
    tm.stop();
    auto d = tm.milli_elapsed() - 250;
    uassert_true(d < 20);  
}

static void test_try_to_lock()
{
    launch_thread_when_locking(mut, 250, f2);
}

static void test_adopt_lock()
{
    {
        typedef std::mutex M;
        M m;
        m.lock();
        std::unique_lock<M> lk(m, std::adopt_lock);
        uassert_true(lk.mutex() == std::addressof(m));
        uassert_true(lk.owns_lock() == true);
    }
    {
        typedef nasty_mutex M;
        M m;
        m.lock();
        std::unique_lock<M> lk(m, std::adopt_lock);
        uassert_true(lk.mutex() == std::addressof(m));
        uassert_true(lk.owns_lock() == true);
    }
}

static void test_defer_lock()
{
    {
        typedef std::mutex M;
        M m;
        std::unique_lock<M> lk(m, std::defer_lock);
        uassert_true(lk.mutex() == std::addressof(m));
        uassert_true(lk.owns_lock() == false);
    }
    {
       typedef nasty_mutex M;
        M m;
        std::unique_lock<M> lk(m, std::defer_lock);
        uassert_true(lk.mutex() == std::addressof(m));
        uassert_true(lk.owns_lock() == false);
    }
}

static void test_unique_lock_cons()
{
    UTEST_UNIT_RUN(test_default_constr);
    UTEST_UNIT_RUN(test_move_constr);
    UTEST_UNIT_RUN(test_move_assign);
    UTEST_UNIT_RUN(test_mutex);
    UTEST_UNIT_RUN(test_try_to_lock);
    UTEST_UNIT_RUN(test_adopt_lock);
    UTEST_UNIT_RUN(test_defer_lock);
}
UTEST_TC_EXPORT(test_unique_lock_cons, "components.cplusplus.mutex.unique_lock.constr", 
                utest_test_init, utest_test_cleanup, 2);
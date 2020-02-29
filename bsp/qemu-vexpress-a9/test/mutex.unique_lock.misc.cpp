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

struct mutex_mock
{
    static int lock_count;
    static int unlock_count;
    void lock() 
    { 
        ++lock_count; 
        rt_kprintf("lock_count: %d\n", lock_count); 
    }
    void unlock() 
    { 
        ++unlock_count; 
        rt_kprintf("unlock_count: %d\n", unlock_count);     
    }
};

int mutex_mock::lock_count = 0;
int mutex_mock::unlock_count = 0;

static mutex_mock m;
static std::mutex mut;

static void test_mutex()
{
    std::unique_lock<std::mutex> lk0;
    uassert_true(lk0.mutex() == nullptr);
    std::unique_lock<std::mutex> lk1(mut);
    uassert_true(lk1.mutex() == &mut);
    lk1.unlock();
    uassert_true(lk1.mutex() == &mut);
}

static void test_operator_bool()
{
    std::unique_lock<std::mutex> lk0;
    uassert_true(static_cast<bool>(lk0) == false);
    std::unique_lock<std::mutex> lk1(mut);
    uassert_true(static_cast<bool>(lk1) == true);
    lk1.unlock();
    uassert_true(static_cast<bool>(lk1) == false);
}

static void test_owns_lock()
{
    std::unique_lock<std::mutex> lk0;
    uassert_true(lk0.owns_lock() == false);
    std::unique_lock<std::mutex> lk1(mut);
    uassert_true(lk1.owns_lock() == true);
    lk1.unlock();
    uassert_true(lk1.owns_lock() == false);
}

static void test_member_swap()
{
    mutex_mock::lock_count = 0;
    mutex_mock::unlock_count = 0;
    std::unique_lock<mutex_mock> lk1(m);
    std::unique_lock<mutex_mock> lk2;
    lk1.swap(lk2);
    uassert_true(lk1.mutex() == nullptr);
    uassert_true(lk1.owns_lock() == false);
    uassert_true(lk2.mutex() == &m);
    uassert_true(lk2.owns_lock() == true);
}

static void test_nonmember_swap()
{
    mutex_mock::lock_count = 0;
    mutex_mock::unlock_count = 0;

    std::unique_lock<mutex_mock> lk1(m);
    std::unique_lock<mutex_mock> lk2;
    std::swap(lk1, lk2);
    uassert_true(lk1.mutex() == nullptr);
    uassert_true(lk1.owns_lock() == false);
    uassert_true(lk2.mutex() == &m);
    uassert_true(lk2.owns_lock() == true);
}

static void test_release()
{
    mutex_mock::lock_count = 0;
    mutex_mock::unlock_count = 0;    

    std::unique_lock<mutex_mock> lk(m);
    uassert_true(lk.mutex() == &m);
    uassert_true(lk.owns_lock() == true);
    uassert_true(mutex_mock::lock_count == 1);
    uassert_true(mutex_mock::unlock_count == 0);
    uassert_true(lk.release() == &m);
    uassert_true(lk.mutex() == nullptr);
    uassert_true(lk.owns_lock() == false);
    uassert_true(mutex_mock::lock_count == 1);
    uassert_true(mutex_mock::unlock_count == 0);
}

static void test_unique_lock_misc()
{
    UTEST_UNIT_RUN(test_member_swap);
    UTEST_UNIT_RUN(test_nonmember_swap);
    UTEST_UNIT_RUN(test_release);

    UTEST_UNIT_RUN(test_mutex);
    UTEST_UNIT_RUN(test_operator_bool);
    UTEST_UNIT_RUN(test_owns_lock);
}
UTEST_TC_EXPORT(test_unique_lock_misc, "components.cplusplus.mutex.unique_lock.misc", 
                utest_test_init, utest_test_cleanup, 2);
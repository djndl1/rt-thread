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

typedef std::chrono::milliseconds ms;


static std::once_flag flg0;
static int init0_called = 0;
static void init0()
{
    std::this_thread::sleep_for(ms(250));
    ++init0_called;
}
static void f0()
{
    std::call_once(flg0, init0);
}



static std::once_flag flg3;
static int init3_called = 0;
static int init3_completed = 0;
static void init3()
{
    ++init3_called;
    std::this_thread::sleep_for(ms(250));
    if (init3_called == 1)
        throw 1;
    ++init3_completed;
}
static void f3()
{
    try
    {
        std::call_once(flg3, init3);
    }
    catch (...)
    {
    }
}

struct init1
{
    static int called;

    void operator()(int i) {called += i;}
};

int init1::called = 0;
static std::once_flag flg1;
static void f1()
{
    std::call_once(flg1, init1(), 1);
}

struct init2
{
    static int called;

    void operator()(int i, int j) const {called += i + j;}
};
int init2::called = 0;
static std::once_flag flg2;
static void f2()
{
    std::call_once(flg2, init2(), 2, 3);
    std::call_once(flg2, init2(), 4, 5);
}


static std::once_flag flg41;
static std::once_flag flg42;
int init41_called = 0;
int init42_called = 0;

static void init42();

static void init41()
{
    std::this_thread::sleep_for(ms(250));
    ++init41_called;
}

static void init42()
{
    std::this_thread::sleep_for(ms(250));
    ++init42_called;
}

static void f41()
{
    std::call_once(flg41, init41);
    std::call_once(flg42, init42);
}

static void f42()
{
    std::call_once(flg42, init42);
    std::call_once(flg41, init41);
}





class MoveOnly
{
#if !defined(__clang__)
   // GCC 4.8 complains about the following being private
public:
    MoveOnly(const MoveOnly&)
    {
    }
#else
    MoveOnly(const MoveOnly&);
#endif
public:
    MoveOnly() {}
    MoveOnly(MoveOnly&&) {}

    void operator()(MoveOnly&&)
    {
    }
};

class NonCopyable
{
#if !defined(__clang__)
   // GCC 4.8 complains about the following being private
public:
    NonCopyable(const NonCopyable&)
    {
    }
#else
    NonCopyable(const NonCopyable&);
#endif
public:
    NonCopyable() {}

    void operator()(int&) {}
};

// reference qualifiers on functions are a C++11 extension
struct RefQual
{
    int lv_called, rv_called;

    RefQual() : lv_called(0), rv_called(0) {}

    void operator()() & 
    { 
        ++lv_called; 
        rt_kprintf("lv_called = %d\n", lv_called);
    }
    void operator()() && 
    { 
        ++rv_called; 
        rt_kprintf("rv_called = %d\n", rv_called);

    }
};

static void test_basic_functionality()
{
        std::thread t0(f0);
        std::thread t1(f0);
        t0.join();
        t1.join();
        uassert_true(init0_called == 1);
}

static void test_basic_exception_safety()
{
        std::thread t0(f3);
        std::thread t1(f3);
        t0.join();
        t1.join();
        uassert_true(init3_called == 2);
        uassert_true(init3_completed == 1);
}

static void test_deadlock_avoidance()
{
        std::thread t0(f41);
        std::thread t1(f42);
        t0.join();
        t1.join();
        uassert_true(init41_called == 1);
        uassert_true(init42_called == 1);
}

static void test_function_with_one_arg()
{
        std::thread t0(f1);
        std::thread t1(f1);
        t0.join();
        t1.join();
        uassert_true(init1::called == 1);
}

static void test_function_with_two_args()
{
        std::thread t0(f2);
        std::thread t1(f2);
        t0.join();
        t1.join();
        uassert_true(init2::called == 5);
}

/* static void test_move_only()
{
        std::once_flag f;
        std::call_once(f, MoveOnly(), MoveOnly());
}
 */
static void test_noncopyable()
{
        std::once_flag f;
        int i = 0;
        std::call_once(f, NonCopyable(), i);
}

static void test_ref_qualifier()
{
        std::once_flag f1, f2;
        RefQual rq;
        std::call_once(f1, rq);
        uassert_true(rq.lv_called == 1);
        rt_kprintf("rq.lv_called = %d\n", rq.lv_called);
        std::call_once(f2, std::move(rq));
        uassert_true(rq.rv_called == 1);
        rt_kprintf("rq.rv_called = %d\n", rq.rv_called);
}

std::once_flag flg01;
long global = 0;

void init01()
{
    ++global;
}

void f01()
{
    std::call_once(flg01, init01);
    uassert_true(global == 1);
}

static void test_race()
{
    std::thread t0(f01);
    std::thread t1(f01);
    t0.join();
    t1.join();
    uassert_true(global == 1);
}

static void test_call_once()
{
    UTEST_UNIT_RUN(test_basic_functionality);
    //UTEST_UNIT_RUN(test_basic_exception_safety);
    UTEST_UNIT_RUN(test_deadlock_avoidance);
    UTEST_UNIT_RUN(test_function_with_one_arg);
    UTEST_UNIT_RUN(test_function_with_two_args);
    //UTEST_UNIT_RUN(test_move_only);
    UTEST_UNIT_RUN(test_noncopyable);
    //UTEST_UNIT_RUN(test_ref_qualifier);
    UTEST_UNIT_RUN(test_race);
}
UTEST_TC_EXPORT(test_call_once, "components.cplusplus.mutex.call_once", 
                utest_test_init, utest_test_cleanup, 2);
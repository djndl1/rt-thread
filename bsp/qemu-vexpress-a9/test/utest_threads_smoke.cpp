#include <rtthread.h>
#include <utest.h>
#include <cstring>
#include <iostream>
#include <ctime>
#include <thread>
#include <cmath>

#include "test_utils.h"

static rt_err_t utest_thread_init(void)
{
    G::n_alive = 0;
    G::op_run = false;
    return RT_EOK;
}

static  rt_err_t utest_thread_cleanup(void)
{
    return RT_EOK;
}


static void test_thread_handle_type(void)
{
        uassert_true((std::is_same<std::thread::native_handle_type, pthread_t>::value));
}

static void test_thread_native_handle(void)
{
    {
        G g{};
        std::thread t0(g);
        pthread_t pid = t0.native_handle();
        uassert_true(pid != PTHREAD_NUM_MAX);
        t0.join();
    }
}

 static void test_sleep_for(void)
{
    std::chrono::milliseconds ms(500);
    clock_t start = clock();

    std::this_thread::sleep_for(ms);

    clock_t end = clock();

    double err = ((double)(end - start)) / RT_TICK_PER_SECOND - 0.5;

    double margin = ((double) ms.count() * 5 / 100) / 1000;

    // The time slept is within 5% of 500ms
    uassert_true(std::abs(err) < margin);
} 

static void test_thread_class(void)
{
    UTEST_UNIT_RUN(test_thread_handle_type);
    UTEST_UNIT_RUN(test_thread_native_handle);
    UTEST_UNIT_RUN(test_sleep_for);
}
UTEST_TC_EXPORT(test_thread_class, "components.cplusplus.thread.smoke", utest_thread_init, utest_thread_cleanup,10);
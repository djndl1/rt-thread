#include <rtthread.h>
#include <utest.h>
#include <thread>

static rt_err_t utest_test_init(void)
{
    return RT_EOK;
}

static  rt_err_t utest_test_cleanup(void)
{
    return RT_EOK;
}

static void test_hardware_concurrency()
{
    uassert_true(std::thread::hardware_concurrency() >= 0);
}

static void test_thread_static()
{
    UTEST_UNIT_RUN(test_hardware_concurrency);
}
UTEST_TC_EXPORT(test_thread_static, "components.cplusplus.thread.static", 
                utest_test_init, utest_test_cleanup, 1);
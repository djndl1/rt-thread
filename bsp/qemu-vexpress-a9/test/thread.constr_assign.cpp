#include <rtthread.h>
#include <utest.h>
#include <thread>

#include "test_utils.h"

static void test_default_constr()
{
    std::thread t;
    uassert_true(t.get_id() == std::thread::id());
}


static rt_err_t utest_test_init(void)
{
    rt_kprintf("Starting to test constructors\n");
    G::n_alive = 0;
    G::op_run = false;
    return RT_EOK;
}

static  rt_err_t utest_test_cleanup(void)
{
    rt_kprintf("Finishing\n");
    return RT_EOK;
}

static void test_move_constr()
{
    {
        G g;
        uassert_true(G::n_alive == 1);
        uassert_true(!G::op_run);
        std::thread t0(g, 5, 5.5);
        std::thread::id id = t0.get_id();
        std::thread t1 = std::move(t0);
        uassert_true(t1.get_id() == id);
        uassert_true(t0.get_id() == std::thread::id());
        t1.join();
        uassert_true(G::n_alive == 1);
        uassert_true(G::op_run);
    }
    uassert_true(G::n_alive == 0);
}

static void test_move_assign()
{
        G::op_run = false;
    {
        uassert_true(G::n_alive == 0);
        uassert_true(!G::op_run);
        {
        G g;
        std::thread t0(g, 5, 5.5);
        std::thread::id id = t0.get_id();
        std::thread t1;
        t1 = std::move(t0);
        uassert_true(t1.get_id() == id);
        uassert_true(t0.get_id() == std::thread::id());
        t1.join();
        }
        uassert_true(G::n_alive == 0);
        uassert_true(G::op_run);
    }
}

static void test_thread_swap()
{
    {
        G g;
        std::thread t0(g);
        std::thread::id id0 = t0.get_id();
        std::thread t1;
        std::thread::id id1 = t1.get_id();
        swap(t0, t1);
        uassert_true(t0.get_id() == id1);
        uassert_true(t1.get_id() == id0);
        t1.join();
    }
}

void f1()
{
    std::exit(0);
}

static void test_thread_destr()
{
    G::op_run = false;
    std::set_terminate(f1);
    {
        uassert_true(G::n_alive == 0);
        uassert_true(!G::op_run);
        G g;
        {
          std::thread t(g);
          std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
    uassert_true(false);
}

static void test_thread_constr()
{
    UTEST_UNIT_RUN(test_default_constr);
    UTEST_UNIT_RUN(test_move_constr);
    UTEST_UNIT_RUN(test_move_assign);
    UTEST_UNIT_RUN(test_thread_swap);
    //UTEST_UNIT_RUN(test_thread_destr);
}
UTEST_TC_EXPORT(test_thread_constr, "components.cplusplus.thread.constr_destr_assign", 
                utest_test_init, utest_test_cleanup, 2);
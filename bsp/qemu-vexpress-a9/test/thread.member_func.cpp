#include <rtthread.h>
#include <utest.h>
#include <thread>
#include <system_error>

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

static void foo() { rt_kprintf("Here is function foo()\n");}

static void test_join()
{
    {     
        G g;
        std::thread t0(g);    
        uassert_true(t0.joinable());
        t0.join();
        uassert_true(!t0.joinable());

        try 
        {
            t0.join();
            uassert_true(false);
        } 
        catch (std::system_error const&) 
        {
        }
    }
          
    {
        std::thread t0(foo);
        t0.detach();
        try
        {
            t0.join();
            uassert_true(false);
        } 
        catch (std::system_error const&) 
        { 

        }
    }
}

static void test_joinable()
{
    {
        G g;
        std::thread t0(g);
        uassert_true(t0.joinable());
        t0.join();
        uassert_true(!t0.joinable());
    }          
}

static void test_detach()
{ 
     {
        G::op_run = false;
        G g;
        std::thread t0(g);
        uassert_true(t0.joinable());
        t0.detach();
        uassert_true(!t0.joinable());
    }
          
    {
        std::thread t0(foo);
        uassert_true(t0.joinable());
        t0.detach();
        uassert_true(!t0.joinable());
        try 
        {
            t0.detach();
        } 
        catch (std::system_error const&) 
        {
        }
    }
}

static void test_get_id()
{
     {
        G g;
        std::thread t0(g);
        std::thread::id id0 = t0.get_id();
        std::thread t1;
        std::thread::id id1 = t1.get_id();
        uassert_true(t0.get_id() == id0);
        uassert_true(id0 != id1);
        uassert_true(t1.get_id() == std::thread::id());
        t0.join();
    }
}

static void test_swap()
{
    {
        G g;
        std::thread t0(g);
        std::thread::id id0 = t0.get_id();
        std::thread t1;
        std::thread::id id1 = t1.get_id();
        t0.swap(t1);
        uassert_true(t0.get_id() == id1);
        uassert_true(t1.get_id() == id0);
        t1.join();
    }
}

static void test_thread_member_func()
{
    UTEST_UNIT_RUN(test_join);
    UTEST_UNIT_RUN(test_joinable);
    UTEST_UNIT_RUN(test_detach);
    UTEST_UNIT_RUN(test_get_id);
    UTEST_UNIT_RUN(test_swap);
}
UTEST_TC_EXPORT(test_thread_member_func, "components.cplusplus.thread.memfunc", 
                utest_test_init, utest_test_cleanup, 2);
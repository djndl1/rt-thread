#include <rtthread.h>
#include <utest.h>
#include <thread>
#include <cmath>
#include <sstream>
#include <string>

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
    std::thread::id id;
    uassert_true(id == std::thread::id());
}

static void test_copy_constr()
{
    std::thread::id id0;
    std::thread::id id1 = id0;
    uassert_true(id1 == id0);
}

static void foo()
{
    std::thread::id id0;
    std::thread::id id1;
    id1 = id0;
    uassert_true(id1 == id0);
    id1 = std::this_thread::get_id();
    uassert_true(id1 != id0);
}

static void test_assignment()
{
    std::thread thr(foo);
    thr.join();
}

static void bar()
{
    std::thread::id id0;
    std::thread::id id1;
    id1 = id0;
    uassert_true( (id1 == id0));
    uassert_true(!(id1 != id0));
    id1 = std::this_thread::get_id();
    uassert_true(!(id1 == id0));
    uassert_true( (id1 != id0));
}

static void test_equality_operator()
{
    std::thread thr(bar);
    thr.join();
}

static void foo2()
{
    std::thread::id id0;
    std::thread::id id1;
    std::thread::id id2 = std::this_thread::get_id();
    uassert_true(!(id0 <  id1));
    uassert_true( (id0 <= id1));
    uassert_true(!(id0 >  id1));
    uassert_true( (id0 >= id1));
    uassert_true(!(id0 == id2));
    if (id0 < id2) {
      uassert_true( (id0 <= id2));
      uassert_true(!(id0 >  id2));
      uassert_true(!(id0 >= id2));
    } else {
      uassert_true(!(id0 <= id2));
      uassert_true( (id0 >  id2));
      uassert_true( (id0 >= id2));
    }
}

static void test_inequality_opertor()
{
    std::thread thr(foo2);
    thr.join();
}

static void bar2()
{
    std::thread::id id0 = std::this_thread::get_id();
    std::ostringstream os;
    os << id0;
    std::string id_str = os.str();
    auto id_int = std::stoul(id_str);
    uassert_in_range(id_int, 0, 8);
}

static void test_stream()
{
    std::thread thr(bar2);
    thr.join();
}

static void foo3()
{
    std::thread::id id1;
    std::thread::id id2 = std::this_thread::get_id();
    typedef std::hash<std::thread::id> H;
    static_assert((std::is_same<typename H::argument_type, std::thread::id>::value), "" );
    static_assert((std::is_same<typename H::result_type, std::size_t>::value), "" );
    H h;
    uassert_true(h(id1) != h(id2));
}

static void test_hash()
{
    std::thread thr(foo3);
    thr.join();
}

static void test_thread_this_thread()
{
    UTEST_UNIT_RUN(test_default_constr);
    UTEST_UNIT_RUN(test_copy_constr);
    UTEST_UNIT_RUN(test_assignment);
    UTEST_UNIT_RUN(test_equality_operator);
    UTEST_UNIT_RUN(test_inequality_opertor);
    UTEST_UNIT_RUN(test_stream);
    UTEST_UNIT_RUN(test_hash);
}
UTEST_TC_EXPORT(test_thread_this_thread, "components.cplusplus.thread.id", 
                utest_test_init, utest_test_cleanup, 2);
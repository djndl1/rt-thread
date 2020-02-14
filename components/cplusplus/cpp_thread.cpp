#include "thread"



#define _RT_NPROCS 0

namespace std
{

    thread::~thread()
    {
        if (joinable()) // when either not joined or not detached
            terminate();
    }

    void thread::join()
    {
        int err = EINVAL;

        if (joinable())
            err = pthread_join(_m_thr.__cpp_thread_t, NULL);
        if (err)
    #ifdef RT_USING_CPP_EXCEPTION
            throw system_error(error_code(err, system_category()), "thread::join failed");
    #else
            abort();
    #endif
        _m_thr = id();
    }

    void thread::detach()
    {
        int err = EINVAL;

        if (joinable())
            err = pthread_detach(_m_thr.__cpp_thread_t);
        if (err)
    #ifdef RT_USING_CPP_EXCEPTION
            throw system_error(error_code(err, system_category()), "thread::detach failed");
    #else
            abort();
    #endif
        _m_thr = id();            
    }

// TODO: not yet actually implemented.
// The standard states that the returned value should only be considered a hint.
unsigned thread::hardware_concurrency() noexcept 
{
    int __n = _RT_NPROCS;
    if (__n < 0)
        __n = 0;
    return __n;
        }
}



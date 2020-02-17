#include "thread"



#define _RT_NPROCS 0

namespace std
{

    static void* execute_native_thread_routine(void *p)
    {
        thread::invoker_base* t = static_cast<thread::invoker_base*>(p);
        thread::invoker_base_ptr local;
        local.swap(t->this_ptr); // tranfer the ownership of the invoker into the thread entry

        local->invoke();

        return NULL;
    }

    void thread::start_thread(invoker_base_ptr b)
    {
        auto raw_ptr = b.get();
        // transfer the ownership of the invoker to the new thread
        raw_ptr->this_ptr = std::move(b); 
        int err = pthread_create(&_m_thr.__cpp_thread_t, NULL,
                &execute_native_thread_routine, raw_ptr);

        if (err) 
        {
            raw_ptr->this_ptr.reset();
        #ifdef RT_USING_CPP_EXCEPTION
            throw system_error(error_code(err, system_category()), "Failed to create a thread");
        #else
            abort();
        #endif
        }
        
    }    

    thread::~thread()
    {
        if (joinable()) // when either not joined or not detached
            terminate();
    }

    void thread::join()
    {
        int err = EINVAL;

      //  if (joinable())
            err = pthread_join(native_handle(), NULL);
        
        if (err) 
        {
        #ifdef RT_USING_CPP_EXCEPTION
            throw system_error(error_code(err, system_category()), "thread::join failed");
        #else
            abort();
        #endif
        }
    
        _m_thr = id();
    }

    void thread::detach()
    {
        int err = EINVAL;

        if (joinable())
            err = pthread_detach(native_handle());
        if (err)
        {
        #ifdef RT_USING_CPP_EXCEPTION
            throw system_error(error_code(err, system_category()), "thread::detach failed");
        #else
            abort();
        #endif
        }
    
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



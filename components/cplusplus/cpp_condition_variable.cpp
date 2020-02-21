#include "condition_variable"

namespace std
{
    void condition_variable::wait(unique_lock<mutex>& lock)
    {
        int err = pthread_cond_wait(&_m_cond, lock.mutex()->native_handle());

        if (err)
        {
            throw_system_error(err, "condition_variable::wait: failed to wait on a condition");
        }
    }




} // namespace std

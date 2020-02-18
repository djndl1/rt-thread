#pragma once

#include <cstdlib>
#include <system_error>

#define RT_USING_CPP_EXCEPTION

inline void throw_system_error(int err, const char* what_msg)
{
#ifdef RT_USING_CPP_EXCEPTION
    throw std::system_error(std::error_code(err, std::system_category()), what_msg);
#else
    (void)err;
    (void)what_msg;
    ::abort();
#endif
}
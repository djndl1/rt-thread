#include "thread"


#define _RT_NPROCS 0

namespace std
{

// TODO: not yet actually implemented.
// The standard states that the returned value should only be considered a hint.
unsigned thread::hardware_concurrency() noexcept {
  int __n = _RT_NPROCS;
  if (__n < 0)
    __n = 0;
  return __n;
    }
}



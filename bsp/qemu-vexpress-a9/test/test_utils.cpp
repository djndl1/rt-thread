#include "test_utils.h"

int G::n_alive = 0;
bool G::op_run = false;

std::atomic_bool done(false);
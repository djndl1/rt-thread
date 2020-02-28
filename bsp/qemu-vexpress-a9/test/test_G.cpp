#include "test_utils.h"

int print_G()
{
    rt_kprintf("G::n_alive = %d\n", G::n_alive);
    rt_kprintf("G::op_run = %d\n", G::op_run);
    return 0;
}
MSH_CMD_EXPORT(print_G, G_state);
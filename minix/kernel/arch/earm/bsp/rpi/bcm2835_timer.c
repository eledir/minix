#include "kernel/kernel.h"
#include "kernel/clock.h"
#include <sys/types.h>
#include <machine/cpu.h>
#include <minix/board.h>
#include <minix/mmio.h>
#include <assert.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include "arch_proto.h"
#include "bsp_timer.h"
#include "bsp_intr.h"

int
bsp_register_timer_handler(const irq_handler_t handler)
{
	panic("bsp_register_timer_handler not implemented");
	return 0;
}

void
bsp_timer_init(unsigned freq)
{
	panic("bsp_timer_init not implemented");
}

void
bsp_timer_stop()
{
	panic("bsp_timer_init not implemented");
}


void
bsp_timer_int_handler()
{
	panic("bsp_timer_int_handler not implemented");
}

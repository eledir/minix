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
#include "rpi_timer_registers.h"
#include "rpi_intr_registers.h"
#include "bsp_intr.h"

int
bsp_register_timer_handler(const irq_handler_t handler)
{
	return 0;
}

/* callback for when the free running clock gets mapped */
int
kern_phys_fr_user_mapped(vir_bytes id, phys_bytes address)
{
	return 0;
}

void
bsp_timer_init(unsigned freq)
{
}

void
bsp_timer_stop()
{
}

void
bsp_timer_int_handler()
{
}

#include <assert.h>
#include <sys/types.h>
#include <machine/cpu.h>
#include <minix/type.h>
#include <minix/board.h>
#include <io.h>

#include "kernel/kernel.h"
#include "kernel/proc.h"
#include "kernel/vm.h"
#include "kernel/proto.h"
#include "arch_proto.h"
#include "bsp_reset.h"

void
bsp_reset_init(void)
{
	panic("bsp_reset_init not implemented");
}

void
bsp_reset(void)
{
	panic("bsp_reset not implemented");
}

void
bsp_poweroff(void)
{
	panic("bsp_poweroff not implemented");
}

void bsp_disable_watchdog(void)
{
	panic("bsp_disable_watchdog not implemented");
}


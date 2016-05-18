/* Implements sys_padconf() for the Raspberry Pi. */

#include "kernel/kernel.h"
#include "arch_proto.h"
#include <sys/types.h>
#include <machine/cpu.h>
#include <minix/mmio.h>
#include <minix/padconf.h>
#include <minix/board.h>
#include <minix/com.h>
#include <assert.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>

#include "bsp_padconf.h"

int
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t value)
{
	return EINVAL;
}

void
bsp_padconf_init(void)
{
	return;
}

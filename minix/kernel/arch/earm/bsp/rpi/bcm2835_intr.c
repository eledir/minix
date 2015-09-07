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
#include "hw_intr.h"

int
intr_init(const int auto_eoi)
{
	if (BOARD_IS_RPI(machine.board_id)) {
		panic("intr_init not implemented");
	} else {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	return 0;
}

void
bsp_irq_handle(void)
{
	/* Function called from assembly to handle interrupts */
	panic("bsp_irq_handle not implemented");
}

void
bsp_irq_unmask(int irq)
{
	panic("bsp_irq_unmask not implemented");
}

void
bsp_irq_mask(const int irq)
{
	panic("bsp_irq_mask not implemented");
}


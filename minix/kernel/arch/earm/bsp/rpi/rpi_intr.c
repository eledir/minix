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

#include "rpi_intr_registers.h"

int
intr_init(const int auto_eoi)
{
	if (BOARD_IS_RPI_3_B(machine.board_id)) {
	} else {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	}

	return 0;
}

void
bsp_irq_handle(void)
{
}

void
bsp_irq_unmask(int irq)
{
}

void
bsp_irq_mask(const int irq)
{
}

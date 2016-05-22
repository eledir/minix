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

static struct rpi3_intr
{
	vir_bytes base;
	int size;
} rpi3_intr;

static kern_phys_map intr_phys_map;

int
intr_init(const int auto_eoi)
{
	if (BOARD_IS_RPI_2_B(machine.board_id) || BOARD_IS_RPI_3_B(machine.board_id)) {
		rpi3_intr.base = RPI3_INTR_BASE;
	} else {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	}

	rpi3_intr.size = 0x1000;	/* 4K */

	kern_phys_map_ptr(rpi3_intr.base, rpi3_intr.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & rpi3_intr.base);

	/* Disable FIQ and all interrupts */
	mmio_write(rpi3_intr.base + RPI3_INTR_FIQ_CTRL, 0);
	mmio_write(rpi3_intr.base + RPI3_INTR_DISABLE1, 0xFFFFFFFF);
	mmio_write(rpi3_intr.base + RPI3_INTR_DISABLE2, 0xFFFFFFFF);
	mmio_write(rpi3_intr.base + RPI3_INTR_DISABLE_BASIC, 0xFF);

	return 0;
}

void
bsp_irq_handle(void)
{
	/* Function called from assembly to handle interrupts */
	uint64_t irq_bit = 1;
	uint64_t irq_pending = mmio_read(rpi3_intr.base + RPI3_INTR_PENDING1) |
	                       ((u64_t)mmio_read(rpi3_intr.base + RPI3_INTR_PENDING2)) << 32;

	/* Scan all interrupts bits */
	for (int irq = 0; irq < 64; irq++) {
		if (irq_bit & irq_pending)
			irq_handle(irq);

		irq_bit <<= 1;
	}

	/* Clear all pending interrupts */
	mmio_write(rpi3_intr.base + RPI3_INTR_PENDING1, irq_pending & 0xFFFFFFFF);
	mmio_write(rpi3_intr.base + RPI3_INTR_PENDING2, irq_pending >> 32);
}

void
bsp_irq_unmask(int irq)
{
	if (irq >= 32)
		mmio_write(rpi3_intr.base + RPI3_INTR_ENABLE2, 1 << (irq - 32));
	else
		mmio_write(rpi3_intr.base + RPI3_INTR_ENABLE1, 1 << irq);
}

void
bsp_irq_mask(const int irq)
{
	if (irq >= 32)
		mmio_write(rpi3_intr.base + RPI3_INTR_DISABLE2, 1 << (irq - 32));
	else
		mmio_write(rpi3_intr.base + RPI3_INTR_DISABLE1, 1 << irq);
}

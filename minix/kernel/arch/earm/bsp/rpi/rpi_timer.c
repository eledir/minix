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

#include "cpufunc_timer.h"

#if 0

static irq_hook_t rpi3_timer_hook;

struct stc_timer
{
	vir_bytes base;
	int irq_nr;
	u32_t freq;
};

static struct stc_timer stc_timer = {
	.base = RPI3_STC_BASE,
	.irq_nr = RPI3_IRQ_TIMER3,
	.freq = 0,
};

static kern_phys_map stc_timer_phys_map;

int
bsp_register_timer_handler(const irq_handler_t handler)
{
	/* Initialize the CLOCK's interrupt hook. */
	rpi3_timer_hook.proc_nr_e = NONE;
	rpi3_timer_hook.irq = stc_timer.irq_nr;

	put_irq_handler(&rpi3_timer_hook, stc_timer.irq_nr, handler);

	/* Prepare next firing of timer */
	bsp_timer_int_handler();

	/* only unmask interrupts after registering */
	bsp_irq_unmask(stc_timer.irq_nr);

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
	kern_phys_map_ptr(stc_timer.base, ARM_PAGE_SIZE,
		    VMMF_UNCACHED | VMMF_WRITE, &stc_timer_phys_map,
		    (vir_bytes) & stc_timer.base);

	stc_timer.freq = freq;
}

void
bsp_timer_stop()
{
	bsp_irq_mask(stc_timer.irq_nr);
}

void
bsp_timer_int_handler()
{
	u32_t next_timestamp = mmio_read(stc_timer.base + STC_CLO);
	next_timestamp += STC_FREQ_HZ / stc_timer.freq;

	mmio_write(stc_timer.base + STC_C3, next_timestamp);

	/* Clear match detected bit */
	mmio_write(stc_timer.base + STC_CS, 0x8);
}

/* Use the free running clock as TSC */
void
read_tsc_64(u64_t * t)
{
	*t = ((u64_t)mmio_read(stc_timer.base + STC_CHI)) << 32;
	*t |= mmio_read(stc_timer.base + STC_CLO);
}

#endif

static irq_hook_t arm_timer_hook;

struct arm_timer
{
	int irq_nr;
	u32_t freq;
};

static struct arm_timer arm_timer = {
 	.irq_nr = RPI3_IRQ_ARMTIMER,
	.freq = 0,
};

static kern_phys_map stc_timer_phys_map;

int
bsp_register_timer_handler(const irq_handler_t handler)
{
	/* Initialize the CLOCK's interrupt hook. */
	arm_timer_hook.proc_nr_e = NONE;
	arm_timer_hook.irq = arm_timer.irq_nr;

	put_irq_handler(&arm_timer_hook, arm_timer.irq_nr, handler);

	/* Prepare next firing of timer */
	bsp_timer_int_handler();

	/* only unmask interrupts after registering */
	bsp_irq_unmask(arm_timer.irq_nr);

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
	arm_timer.freq = freq;
}

void
bsp_timer_stop()
{
	bsp_irq_mask(arm_timer.irq_nr);
}

void
bsp_timer_int_handler()
{
	write_cntv_cval(-1);
	write_cntv_tval(read_cntfrq() / arm_timer.freq);
	write_cntv_ctl(ARMTIMER_ENABLE);
}

/* Use the free running clock as TSC */
void
read_tsc_64(u64_t * t)
{
	*t = read_cntv_cval();
}

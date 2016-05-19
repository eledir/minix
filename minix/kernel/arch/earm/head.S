#include "kernel/kernel.h" /* configures the kernel */

/* sections */

#include <arm/armreg.h>
#include <machine/vm.h>
#include "kernel/kernel.h"
#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include <machine/asm.h>
#include <machine/interrupt.h>
#include "archconst.h"
#include "kernel/const.h"
#include "kernel/proc.h"
#include "sconst.h"
#include <machine/multiboot.h>
#include <machine/cpu.h>

#include "arch_proto.h" /* K_STACK_SIZE */

.section .startup
/*===========================================================================*/
/*					MINIX				     */
/*===========================================================================*/
.global MINIX
MINIX:
/* this is the entry point for the MINIX kernel */
	b multiboot_init

.text
multiboot_init:
#if 0
	/* Leave HYP mode and move into supervisor mode with IRQs/FIQs disabled. */
	mrs	r4, cpsr
	and	r4, r4, #(PSR_MODE)	/* Mode is in the low 5 bits of CPSR */
	teq	r4, #(PSR_HYP32_MODE)	/* Hyp Mode? */
	bne	1f

	/* Ensure that IRQ, and FIQ will be disabled after eret */
	mrs	r4, cpsr
	bic	r4, r4, #(PSR_MODE)
	orr	r4, r4, #(PSR_SVC32_MODE)
	orr	r4, r4, #(I32_bit | F32_bit)
	msr	spsr_cxsf, r4
	/* Exit hypervisor mode */
	adr	lr, 2f
	msr	elr_hyp, lr
	/*eret*/

1:
	cpsid	if, #PSR_SVC32_MODE		// SVC32 with no interrupts

2:
#endif
	ldr	sp, =load_stack_start	/* make usable stack */
	mov	fp, #0
	bl	_C_LABEL(pre_init)

	/* Kernel is mapped high now and ready to go, with
	 * the boot info pointer returned by pre_init in r0.
	 * Set the highly mapped stack and initialize it.
	 *
	 * Afther that call kmain with r0 still pointing to boot info
	 */
	ldr	sp, =k_initial_stktop
	mov	r1, #0
	push	{r1}			/* Terminate stack */
	ldr	r2, =_C_LABEL(kmain)	/* r0 holds kinfo_t ptr */
	bx	r2

	/* not reached */
hang:
	b hang

.data
load_stack:
	.space 4096
load_stack_start:

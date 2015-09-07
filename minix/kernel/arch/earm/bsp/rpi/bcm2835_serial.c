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

#include "bcm2835_serial.h"

struct bcm2835_serial
{
	vir_bytes base;
	vir_bytes size;
};

static struct bcm2835_serial bcm2835_serial = {
	.base = BCM2835_UART0_BASE,
};

static kern_phys_map serial_phys_map;

static void delay(int cycles)
{
	volatile int count = cycles;
	while (count--);
}

/*
 * In kernel serial for the bcm2835. See the omap code for the long story.
 */
void
bsp_ser_init()
{
	bcm2835_serial.base = BCM2835_UART0_BASE;
	bcm2835_serial.size = 0x1000;	/* 4k */

	kern_phys_map_ptr(bcm2835_serial.base, bcm2835_serial.size,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_bytes) & bcm2835_serial.base);
	assert(bcm2835_serial.base);

	/* Disable UART0 */
	mmio_write(bcm2835_serial.base + BCM2835_CR, 0);

	/* Disable all pull up/down pins */
	mmio_write(BCM2835_GPPUD, 0);
	delay(150);

	/* Disable pull up/down for pin 14 and 15 */
	mmio_write(BCM2835_GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);

	/* Take effect */
	mmio_write(BCM2835_GPPUDCLK0, 0);

	/* Mask interrupts */
	mmio_write(bcm2835_serial.base + BCM2835_IMSC, BCM2835_OEIM |
	    BCM2835_BEIM | BCM2835_PEIM | BCM2835_FEIM | BCM2835_RTIM |
	    BCM2835_TXIM | BCM2835_RXIM | BCM2835_CTSMIM);

	/* Clear pending interrupts */
	mmio_write(bcm2835_serial.base + BCM2835_ICR, 0x7FF);

	/* Enable UART0 */
	mmio_write(bcm2835_serial.base + BCM2835_CR, BCM2835_RXE | BCM2835_RXE |
	    BCM2835_UARTEN);
}

void
bsp_ser_putc(char c)
{
	int i;
	assert(bcm2835_serial.base);

	/* Wait until FIFO's empty */
	for (i = 0; i < 100000; i++) {
		if (mmio_read(bcm2835_serial.base+BCM2835_FR) & BCM2835_TXFF) {
			break;
		}
	}

	/* Write character */
	mmio_write(bcm2835_serial.base + BCM2835_DR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	for (i = 0; i < 100000; i++) {
		if (mmio_read(bcm2835_serial.base+BCM2835_FR) & BCM2835_TXFF) {
			break;
		}
	}
}

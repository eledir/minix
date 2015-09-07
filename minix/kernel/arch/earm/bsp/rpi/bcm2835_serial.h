#ifndef _BCM2835_SERIAL_H
#define _BCM2835_SERIAL_H

/* GPIO stuff */
#define BCM2835_GPIO_BASE 0x20200000               /* GPIO base address */
#define BCM2835_GPPUD     (BCM2835_GPIO_BASE+0x94) /* Whole GPIO actuation control */
#define BCM2835_GPPUDCLK0 (BCM2835_GPIO_BASE+0x98) /* Specific GPIO actuation control */

/* UART register map */
#define BCM2835_UART0_BASE (BCM2835_GPIO_BASE+0x1000) /* UART0 physical address */

/* UART registers */
#define BCM2835_DR   0x000 /* Data register */
#define BCM2835_FR   0x018 /* Flag register */
#define BCM2835_CR   0x030 /* Control register */
#define BCM2835_IMSC 0x038 /* Interrupt mask set clear register */
#define BCM2835_ICR  0x044 /* Interrupt clear register */

/* Flag register fields */
#define BCM2835_TXFF (1 << 5) /* Transmit FIFO full */
#define BCM2835_RXFE (1 << 4) /* Receive FIFO empty */

/* Control register fields */
#define BCM2835_RXE    (1 << 9) /* Enable reception */
#define BCM2835_TXE    (1 << 8) /* Enable transmition */
#define BCM2835_UARTEN (1 << 0) /* Enable UART */

/* IMSC register fields */
#define BCM2835_OEIM   (1 << 10) /* Overrun error interrupt mask */
#define BCM2835_BEIM   (1 << 9)  /* Break error interrupt mask */
#define BCM2835_PEIM   (1 << 8)  /* Parity error interrupt mask */
#define BCM2835_FEIM   (1 << 7)  /* Framing error interrupt mask */
#define BCM2835_RTIM   (1 << 6)  /* Receive tiemout interrupt mask */
#define BCM2835_TXIM   (1 << 5)  /* Transmit interrupt mask */
#define BCM2835_RXIM   (1 << 4)  /* Receive interrupt mask */
#define BCM2835_CTSMIM (1 << 1)  /* Modem interrupt mask */

#ifndef __ASSEMBLY__

void bcm2835_ser_init();
void bcm2835_ser_putc(char c);

#endif /* __ASSEMBLY__ */

#endif /* _OMAP_SERIAL_H */

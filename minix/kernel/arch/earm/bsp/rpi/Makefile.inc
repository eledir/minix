#
# BSP for Raspberry Pi
#

.PATH: ${.CURDIR}/arch/${MACHINE_ARCH}/bsp/rpi

.for unpaged_obj in bcm2835_serial.o bcm2835_reset.o
BSP_OBJS_UNPAGED += ${unpaged_obj}
.endfor

SRCS+=	bcm2835_init.c bcm2835_serial.c bcm2835_timer.c bcm2835_padconf.c bcm2835_intr.c \
	bcm2835_reset.c


# Makefile for terminal driver (TTY)
PROG=	tty

.include "arch/${MACHINE_ARCH}/Makefile.inc"

SRCS +=	tty.c

DPADD+=	${LIBCHARDRIVER} ${LIBSYS} ${LIBTIMERS}
LDADD+=	-lchardriver -lsys -ltimers -Ttext 0x3800000

SUBDIR= keymaps

# Needs kernel/const.h, etc
CPPFLAGS+=	-I${.CURDIR}	-I${NETBSDSRCDIR} -g -O0

.include <minix.service.mk>
.include <bsd.subdir.mk>

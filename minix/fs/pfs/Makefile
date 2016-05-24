# Makefile for Pipe File System (PFS)
PROG=	pfs
SRCS=	pfs.c

DPADD+=	${LIBFSDRIVER} ${LIBSYS}
LDADD+=	-lfsdriver -lsys -Ttext 0x5000000

.include <minix.service.mk>

# Makefile for Scheduler (SCHED)
PROG=	sched
SRCS=	main.c schedule.c utility.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys -Ttext 0x2000000

.include <minix.service.mk>

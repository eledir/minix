# Makefile for IPC server
PROG=	ipc
SRCS=	main.c utility.c posix_sem.c shm.c sem.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

FILES=ipc.conf
FILESNAME=ipc
FILESDIR= /etc/system.conf.d

WARNS?=	5

.include <minix.service.mk>

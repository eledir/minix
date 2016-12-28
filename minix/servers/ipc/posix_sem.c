#include "inc.h"

#include <semaphore.h>

#define POSIX_SEMAPHORE_NAME_MAX        64
#define MAX_POSIX_SEMAPHORES            NR_PROCS

struct posix_sem_iproc {
	endpoint_t ip_endpt;                    /* process endpoint */
	TAILQ_ENTRY(posix_sem_iproc) ip_next;   /* next waiting process */
} posix_sem_iprocs[NR_PROCS];

struct posix_semaphore {
	char name[POSIX_SEMAPHORE_NAME_MAX];            /* semaphore name */
	int value;                                      /* semaphore value */
	TAILQ_HEAD(waiters, posix_sem_iproc) waiters;   /* processes waiting */
} posix_sems[MAX_POSIX_SEMAPHORES];

int
do_posixsem_close(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_destroy(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_getvalue(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_init(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_post(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_timedwait(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_trywait(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_unlink(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_wait(message *m)
{
	return ENOTSUP;
}

int
do_posixsem_open(message *m)
{
	return ENOTSUP;
}

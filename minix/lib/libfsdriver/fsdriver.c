
#include "fsdriver.h"

/* Library-local variables. */
dev_t fsdriver_device;
ino_t fsdriver_root;
int fsdriver_mounted = FALSE;

static int fsdriver_running;
int fsdriver_flags = 0;

static int in_group(const vfs_ucred_t *ucred, gid_t grp)
{
	int i;

	for (i = 0; i < ucred->vu_ngroups; i++)
		if (ucred->vu_sgroups[i] == grp)
			return OK;

	return EINVAL;
}

/*
 * Process an incoming VFS request, and send a reply.  If the message is not
 * a file system request from VFS, pass it on to the generic message handler.
 * Multithreaded file systems should indicate that the reply is to be sent to
 * VFS asynchronously.
 */
void
fsdriver_process(const struct fsdriver * __restrict fdp,
	const message * __restrict m_ptr, int ipc_status, int asyn_reply)
{
	message m_out;
	unsigned int call_nr;
	int r, transid;

	/* Is this a file system request at all? */
	if (is_ipc_notify(ipc_status) || m_ptr->m_source != VFS_PROC_NR) {
		if (fdp->fdr_other != NULL)
			fdp->fdr_other(m_ptr, ipc_status);

		return; /* do not send a reply */
	}

	/* Call the appropriate function. */
	transid = TRNS_GET_ID(m_ptr->m_type);
	call_nr = TRNS_DEL_ID(m_ptr->m_type);

	memset(&m_out, 0, sizeof(m_out));

	if (fsdriver_mounted || call_nr == REQ_READSUPER) {
		call_nr -= FS_BASE;	/* unsigned; wrapping is intended */

		if (call_nr < NREQS && fsdriver_callvec[call_nr] != NULL)
			r = (fsdriver_callvec[call_nr])(fdp, m_ptr, &m_out);
		else
			r = ENOSYS;
	} else
		r = EINVAL;

	/* Send a reply. */
	m_out.m_type = TRNS_ADD_ID(r, transid);

	if (asyn_reply)
		r = asynsend(m_ptr->m_source, &m_out);
	else
		r = ipc_send(m_ptr->m_source, &m_out);

	if (r != OK)
		printf("fsdriver: sending reply failed (%d)\n", r);

	if (fdp->fdr_postcall != NULL)
		fdp->fdr_postcall();
}

/*
 * Terminate the file server as soon as the file system has been unmounted.
 */
void
fsdriver_terminate(void)
{

	fsdriver_running = FALSE;

	sef_cancel();
}

/*
 * Main program of any file server task.
 */
void
fsdriver_task(struct fsdriver * fdp, int flags)
{
	message mess;
	int r, ipc_status;

	fsdriver_running = TRUE;
	fsdriver_flags = flags;

	while (fsdriver_running || fsdriver_mounted) {
		if ((r = sef_receive_status(ANY, &mess, &ipc_status)) != OK) {
			if (r == EINTR)
				continue;	/* sef_cancel() was called */

			panic("fsdriver: sef_receive_status failed: %d", r);
		}

		fsdriver_process(fdp, &mess, ipc_status, FALSE /*asyn_reply*/);
	}
}

/*
 * Provide file servers with default POSIX authorization checks for access.
 */
int fsdriver_posix_check_access(const struct fsdriver * __restrict fdp,
	ino_t ino_nr, const vfs_ucred_t *ucred,	mode_t access_desired)
{
/* Given a pointer to an vnode, 'vp', and the access desired, determine
 * if the access is allowed, and if not why not.  The routine looks up the
 * caller's uid in the 'fproc' table.  If access is allowed, OK is returned
 * if it is forbidden, EACCES is returned.
 */

	struct stat buf;
	register mode_t bits, perm_bits;
	uid_t uid;
	gid_t gid;
	int r, shift;

	/* Try to stat() the file. */
	if ((r = fdp->fdr_stat(ino_nr, &buf)) != OK)
		return r;

	/* Isolate the relevant rwx bits from the mode. */
	bits = buf.st_mode;
	uid = ucred->vu_uid;
	gid = ucred->vu_gid;

	if ((access_desired & PATH_CHECK_LOOKUP)) {
		if (!S_ISDIR(bits))
			return ENOTDIR;
		else
			access_desired |= X_BIT;
	}
	access_desired &= 07;

	if (uid == ROOT_UID) {
		/* 
		 * Grant read and write permission.  Grant search permission for
		 * directories.  Grant execute permission (for non-directories)
		 * if and only if one of the 'X' bits is set.
		 */
		if ( S_ISDIR(bits) ||
		     bits & ((X_BIT << 6) | (X_BIT << 3) | X_BIT))
			perm_bits = R_BIT | W_BIT | X_BIT;
		else
			perm_bits = R_BIT | W_BIT;
	} else {
		if (uid == buf.st_uid)
			shift = 6;              /* owner */
		else if (gid == buf.st_gid)
			shift = 3;              /* group */
		else if (in_group(ucred, buf.st_gid) == OK)
			shift = 3;              /* suppl. groups */
		else
			shift = 0;              /* other */
		perm_bits = (bits >> shift) & (R_BIT | W_BIT | X_BIT);
	}

	/* If access desired is not a subset of what is allowed, it is refused. 
*/
	r = OK;
	//printf("UID:%d\t PERM:%o\tACCESS:%o\n", uid, perm_bits, access_desired);
	if ((perm_bits | access_desired) != perm_bits) r = EACCES;

	return r;
}

/*
 * Provide file servers with default POSIX authorization checks for chown.
 */
int fsdriver_posix_check_chown(const struct fsdriver * __restrict fdp,
	ino_t ino_nr, const vfs_ucred_t *ucred,	uid_t uid, gid_t gid)
{
	int r = OK;
	struct stat buf;

	/* Try to stat() the file. */
	if ((r = fdp->fdr_stat(ino_nr, &buf)) != OK)
		return r;

	if (ucred->vu_uid != ROOT_UID) {
		/* Regular users can only change groups of their own files. */
		if (buf.st_uid != ucred->vu_uid) r = EPERM;
		if (buf.st_uid != uid) r = EPERM;	/* no giving away */
		if (ucred->vu_gid != gid) r = EPERM;
	}

	return r;
}

/*
 * Provide file servers with default POSIX authorization checks for chown.
 */
int fsdriver_posix_check_chmod(const struct fsdriver * __restrict fdp,
	ino_t ino_nr, const vfs_ucred_t *ucred)
{
	int r = OK;
	struct stat buf;

	/* Try to stat() the file. */
	if ((r = fdp->fdr_stat(ino_nr, &buf)) != OK)
		return r;

	/* 
	 * Only the owner or the super_user may change the mode of a file.
	 * No one may change the mode of a file on a read-only file system.
	 */
	if (buf.st_uid != ucred->vu_uid && ucred->vu_uid != ROOT_UID)
		r = EPERM;

	return r;
}

int fsdriver_posix_check_utime(const struct fsdriver * __restrict fdp,
	ino_t ino_nr, const vfs_ucred_t *ucred)
{
	int r = OK;
	struct stat buf;

	/* Try to stat() the file. */
	if ((r = fdp->fdr_stat(ino_nr, &buf)) != OK)
		return r;

	/* Only the owner of a file or the super user can change timestamps. */
	if (buf.st_uid != ucred->vu_uid && ucred->vu_uid != ROOT_UID)
		r = EPERM;
	/* Need write permission (or super user) to 'touch' the file */
	if (r != OK)
		r = fsdriver_posix_check_access(fdp, ino_nr, ucred, W_BIT);

	return r;
}
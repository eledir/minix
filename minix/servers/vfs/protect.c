/* This file deals with protection in the file system.  It contains the code
 * for four system calls that relate to protection.
 *
 * The entry points into this file are
 *   do_chmod:	perform the CHMOD and FCHMOD system calls
 *   do_chown:	perform the CHOWN and FCHOWN system calls
 *   do_umask:	perform the UMASK system call
 *   do_access:	perform the ACCESS system call
 */

#include "fs.h"
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <minix/callnr.h>
#include "file.h"
#include "path.h"
#include <minix/vfsif.h>
#include "vnode.h"
#include "vmnt.h"

/*===========================================================================*
 *				do_chmod				     *
 *===========================================================================*/
int do_chmod(void)
{
/* Perform the chmod(name, mode) and fchmod(fd, mode) system calls.
 * syscall might provide 'name' embedded in the message.
 */

  struct filp *flp;
  struct vnode *vp;
  struct vmnt *vmp;
  int r, rfd;
  mode_t result_mode;
  char fullpath[PATH_MAX];
  struct lookup resolve;
  mode_t new_mode;

  flp = NULL;

  lookup_init(&resolve, fullpath, PATH_NOFLAGS, &vmp, &vp);
  resolve.l_vmnt_lock = VMNT_READ;
  resolve.l_vnode_lock = VNODE_WRITE;

  if (job_call_nr == VFS_CHMOD) {
	new_mode = job_m_in.m_lc_vfs_path.mode;
	/* Temporarily open the file */
	if (copy_path(fullpath, sizeof(fullpath)) != OK)
		return(err_code);
	if ((vp = eat_path(&resolve, fp)) == NULL) return(err_code);
  } else {	/* call_nr == VFS_FCHMOD */
	rfd = job_m_in.m_lc_vfs_fchmod.fd;
	new_mode = job_m_in.m_lc_vfs_fchmod.mode;
	/* File is already opened; get a pointer to vnode from filp. */
	if ((flp = get_filp(rfd, VNODE_WRITE)) == NULL) return(err_code);
	vp = flp->filp_vno;
        assert(vp);
	dup_vnode(vp);
  }

  assert(vp);

  /* Only the owner or the super_user may change the mode of a file.
   * No one may change the mode of a file on a read-only file system.
   */
  if (vp->v_uid != fp->fp_effuid && fp->fp_effuid != SU_UID)
	r = EPERM;
  else
	r = read_only(vp);

  if (r == OK) {
	/* Now make the change. Clear setgid bit if file is not in caller's
	 * group */
	if (fp->fp_effuid != SU_UID && vp->v_gid != fp->fp_effgid)
		new_mode &= ~I_SET_GID_BIT;

	r = req_chmod(vp->v_fs_e, vp->v_inode_nr, new_mode, &result_mode, fp);
	if (r == OK)
		vp->v_mode = result_mode;
  }

  if (job_call_nr == VFS_CHMOD) {
	unlock_vnode(vp);
	unlock_vmnt(vmp);
  } else {	/* VFS_FCHMOD */
	unlock_filp(flp);
  }

  put_vnode(vp);
  return(r);
}


/*===========================================================================*
 *				do_chown				     *
 *===========================================================================*/
int do_chown(void)
{
/* Perform the chown(path, owner, group) and fchmod(fd, owner, group) system
 * calls. */
  struct filp *flp;
  struct vnode *vp;
  struct vmnt *vmp;
  int r, rfd;
  uid_t uid, new_uid;
  gid_t gid, new_gid;
  mode_t new_mode;
  char fullpath[PATH_MAX];
  struct lookup resolve;
  vir_bytes vname1;
  size_t vname1_length;

  flp = NULL;
  uid = job_m_in.m_lc_vfs_chown.owner;
  gid = job_m_in.m_lc_vfs_chown.group;

  if (job_call_nr == VFS_CHOWN) {
	vname1 = job_m_in.m_lc_vfs_chown.name;
	vname1_length = job_m_in.m_lc_vfs_chown.len;

	lookup_init(&resolve, fullpath, PATH_NOFLAGS, &vmp, &vp);
	resolve.l_vmnt_lock = VMNT_READ;
	resolve.l_vnode_lock = VNODE_WRITE;

	/* Temporarily open the file. */
	if (fetch_name(vname1, vname1_length, fullpath) != OK)
		return(err_code);
	if ((vp = eat_path(&resolve, fp)) == NULL) return(err_code);
  } else {	/* call_nr == VFS_FCHOWN */
	rfd = job_m_in.m_lc_vfs_chown.fd;

	/* File is already opened; get a pointer to the vnode from filp. */
	if ((flp = get_filp(rfd, VNODE_WRITE)) == NULL)
		return(err_code);
	vp = flp->filp_vno;
	dup_vnode(vp);
  }

  r = read_only(vp);
  if (r == OK) {
	/* FS is R/W. Whether call is allowed depends on ownership, etc. */
	/* The super user can do anything, so check permissions only if we're
	   a regular user. */
	if (fp->fp_effuid != SU_UID) {
		/* Regular users can only change groups of their own files. */
		if (vp->v_uid != fp->fp_effuid) r = EPERM;
		if (vp->v_uid != uid) r = EPERM;	/* no giving away */
		if (fp->fp_effgid != gid) r = EPERM;
	}
  }

  if (r == OK) {
	/* Do not change uid/gid if new uid/gid is -1. */
	new_uid = (uid == (uid_t)-1 ? vp->v_uid : uid);
	new_gid = (gid == (gid_t)-1 ? vp->v_gid : gid);

	if (new_uid > UID_MAX || new_gid > GID_MAX)
		r = EINVAL;
	else if ((r = req_chown(vp->v_fs_e, vp->v_inode_nr, new_uid, new_gid,
				&new_mode, fp)) == OK) {
		vp->v_uid = new_uid;
		vp->v_gid = new_gid;
		vp->v_mode = new_mode;
	}
  }

  if (job_call_nr == VFS_CHOWN) {
	unlock_vnode(vp);
	unlock_vmnt(vmp);
  } else {	/* VFS_FCHOWN */
	unlock_filp(flp);
  }

  put_vnode(vp);
  return(r);
}

/*===========================================================================*
 *				do_umask				     *
 *===========================================================================*/
int do_umask(void)
{
/* Perform the umask(2) system call. */
  mode_t complement, new_umask;

  new_umask = job_m_in.m_lc_vfs_umask.mask;

  complement = ~fp->fp_umask;	/* set 'r' to complement of old mask */
  fp->fp_umask = ~(new_umask & RWX_MODES);
  return(complement);		/* return complement of old mask */
}


/*===========================================================================*
 *				do_access				     *
 *===========================================================================*/
int do_access(void)
{
/* Perform the access(name, mode) system call.
 * syscall might provide 'name' embedded in the message.
 */
  int r;
  struct vnode *vp;
  struct vmnt *vmp;
  char fullpath[PATH_MAX];
  struct lookup resolve;
  mode_t access;

  access = job_m_in.m_lc_vfs_path.mode;

  lookup_init(&resolve, fullpath, access & 07, &vmp, &vp);
  resolve.l_vmnt_lock = VMNT_READ;
  resolve.l_vnode_lock = VNODE_READ;

  /* First check to see if the mode is correct. */
  if ( (access & ~(R_OK | W_OK | X_OK)) != 0 && access != F_OK)
	return(EINVAL);

  /* Temporarily open the file. */
  if (copy_path(fullpath, sizeof(fullpath)) != OK)
	return(err_code);
  if ((vp = eat_path(&resolve, fp)) == NULL) return(err_code);

  r = forbidden(fp, vp, access);

  unlock_vnode(vp);
  unlock_vmnt(vmp);

  put_vnode(vp);
  return(r);
}


/*===========================================================================*
 *				forbidden				     *
 *===========================================================================*/
int forbidden(struct fproc *rfp, struct vnode *vp, mode_t access_desired)
{
/* Given a pointer to an vnode, 'vp', and the access desired, determine
 * if the access is allowed, and if not why not.  The routine looks up the
 * caller's uid in the 'fproc' table.  If access is allowed, OK is returned
 * if it is forbidden, EACCES is returned.
 */
  int r = OK;

  (void)rfp;

  if (r == OK)
	if (access_desired & W_BIT)
		r = read_only(vp);

  return(r);
}

/*===========================================================================*
 *				read_only				     *
 *===========================================================================*/
int read_only(vp)
struct vnode *vp;		/* ptr to inode whose file sys is to be cked */
{
/* Check to see if the file system on which the inode 'ip' resides is mounted
 * read only.  If so, return EROFS, else return OK.
 */
  assert(vp);
  return(vp->v_vmnt && (vp->v_vmnt->m_flags & VMNT_READONLY) ? EROFS : OK);
}

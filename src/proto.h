/* vi: set sw=4 ts=4: */
/*
 * libfakechroot -- fake chroot environment
 * (c) 2003-2005 Piotr Roszatycki <dexter@debian.org>, LGPL
 * (c) 2006, 2007 Alexander Shishkin <virtuoso@slind.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or(at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifndef __FAKECHROOT_PROTO_H__
#define __FAKECHROOT_PROTO_H__

#include "wrapper.h"

/* execXX() call family */
WRAPPER_PROTO(execl,  int, (const char *path, const char *arg, ...))
WRAPPER_PROTO(execle, int, (const char *path, const char *arg, ...))
WRAPPER_PROTO(execlp, int, (const char *file, const char *arg, ...))
WRAPPER_PROTO(execv,  int, (const char *path, char *const argv []))
WRAPPER_PROTO(execve, int, (const char *filename, char *const argv [], char *const envp[]))
WRAPPER_PROTO(execvp, int, (const char *file, char *const argv[]))

WRAPPER_PROTO(access, int, (const char *pathname, int mode))
WRAPPER_PROTO(acct, int, (const char *filename))
WRAPPER_PROTO(chdir, int, (const char *path))
WRAPPER_PROTO(chmod, int, (const char *path, mode_t mode))
WRAPPER_PROTO(chown, int, (const char *path, uid_t owner, gid_t group))
WRAPPER_PROTO(chroot, int, (const char *path))
WRAPPER_PROTO(creat, int, (const char *path, mode_t mode))
WRAPPER_PROTO(creat64, int, (const char *path, mode_t mode))
WRAPPER_PROTO(dlopen, void *, (const char *path, int flag))
WRAPPER_PROTO(fopen, FILE *, (const char *path, const char *mode))
WRAPPER_PROTO(fopen64, FILE *, (const char *path, const char *mode))
WRAPPER_PROTO(freopen, FILE *, (const char *path, const char *mode, FILE *stream))
WRAPPER_PROTO(freopen64, FILE *, (const char *path, const char *mode, FILE *stream))
WRAPPER_PROTO(getcwd, char *, (char *buf, size_t size))
WRAPPER_PROTO(getwd, char *, (char *buf))
WRAPPER_PROTO(glob, int, (const char *pattern, int flags, int(*errfunc) (const char *, int), glob_t *pglob))
WRAPPER_PROTO(lchown, int, (const char *path, uid_t owner, gid_t group))
WRAPPER_PROTO(link, int, (const char *oldpath, const char *newpath))
WRAPPER_PROTO(mkdir, int, (const char *path, mode_t mode))
WRAPPER_PROTO(mkdirat, int, (int dirfd, const char *pathname, mode_t mode))
WRAPPER_PROTO(mkfifo, int, (const char *path, mode_t mode))
WRAPPER_PROTO(mkstemp, int, (char *template))
WRAPPER_PROTO(mkstemp64, int, (char *template))
WRAPPER_PROTO(mktemp, char *, (char *template))
WRAPPER_PROTO(open, int, (const char *pathname, int flags, ...))
WRAPPER_PROTO(open64, int, (const char *pathname, int flags, ...))
WRAPPER_PROTO(openat, int, (int dirfd, const char *pathname, int flags, ...))
WRAPPER_PROTO(openat64, int, (int dirfd, const char *pathname, int flags, ...))
WRAPPER_PROTO(opendir, DIR *, (const char *name))
WRAPPER_PROTO(pathconf, long, (const char *pathname, int name))
WRAPPER_PROTO(readlink, ssize_t, (const char *path, char *buf, READLINK_TYPE_ARG3))
WRAPPER_PROTO(realpath, char *, (const char *path, char *resolved))
WRAPPER_PROTO(remove, int, (const char *path))
WRAPPER_PROTO(rename, int, (const char *oldpath, const char *newpath))
WRAPPER_PROTO(renameat, int, (int olddirfd, const char *oldpath, int newdirfd, const char *newpath))
WRAPPER_PROTO(rmdir, int, (const char *path))
WRAPPER_PROTO(symlink, int, (const char *oldpath, const char *newpath))
WRAPPER_PROTO(tempnam, char *, (const char *dir, const char *pfx))
WRAPPER_PROTO(tmpnam, char *, (char *dir))
WRAPPER_PROTO(truncate, int, (const char *path, off_t offset))
WRAPPER_PROTO(unlink, int, (const char *path))
WRAPPER_PROTO(unlinkat, int, (int dirfd, const char *pathname, int flags))
WRAPPER_PROTO(utime, int, (const char *filename, const struct utimbuf *buf))
WRAPPER_PROTO(utimes, int, (const char *filename, const struct timeval tv[2]))
//WRAPPER_PROTO(, int, ())

WRAPPER_PROTO(__fxstatat, int, (int ver, int drifd, const char *pathname, struct stat *buf, int flags))
WRAPPER_PROTO(__fxstatat64, int, (int ver, int drifd, const char *pathname, struct stat64 *buf, int flags))
WRAPPER_PROTO(__lxstat, int, (int ver, const char *filename, struct stat *buf))
WRAPPER_PROTO(__lxstat64, int, (int ver, const char *filename, struct stat64 *buf))
WRAPPER_PROTO(__open, int, (const char *pathname, int flags, ...))
WRAPPER_PROTO(__open64, int, (const char *pathname, int flags, ...))
WRAPPER_PROTO(__opendir2, DIR *, (const char *name, int flags))
WRAPPER_PROTO(__xmknod, int, (int ver, const char *path, mode_t mode, dev_t *dev))
WRAPPER_PROTO(__xstat, int, (int ver, const char *filename, struct stat *buf))
WRAPPER_PROTO(__xstat64, int, (int ver, const char *filename, struct stat64 *buf))
WRAPPER_PROTO(_xftw, int, (int mode, const char *dir, int(*fn)(const char *file, const struct stat *sb, int flag), int nopenfd))
WRAPPER_PROTO(_xftw64, int, (int mode, const char *dir, int(*fn)(const char *file, const struct stat64 *sb, int flag), int nopenfd))
WRAPPER_PROTO(canonicalize_file_name, char *, (const char *name))
WRAPPER_PROTO(dlmopen, void *, (Lmid_t nsid, const char *filename, int flag))
WRAPPER_PROTO(eaccess, int, (const char *pathname, int mode))
WRAPPER_PROTO(euidaccess, int, (const char *pathname, int mode))
WRAPPER_PROTO(fts_open, FTS *, (char * const *path_argv, int options,
		int(*compar)(const FTSENT **, const FTSENT **)))
WRAPPER_PROTO(ftw, int, (const char *dir, int(*fn)(const char *file, const struct stat *sb, int flag), int nopenfd))
WRAPPER_PROTO(ftw64, int, (const char *dir, int(*fn)(const char *file, const struct stat64 *sb, int flag), int nopenfd))
WRAPPER_PROTO(get_current_dir_name, char *, (void))
WRAPPER_PROTO(getxattr, ssize_t, (const char *path, const char *name, void *value, size_t size))
WRAPPER_PROTO(glob64, int, (const char *pattern, int flags, int(*errfunc) (const char *, int),
		glob64_t *pglob))
WRAPPER_PROTO(glob_pattern_p, int, (const char *pattern, int quote))
WRAPPER_PROTO(lchmod, int, (const char *path, mode_t mode))
WRAPPER_PROTO(lckpwdf, int, (void))
WRAPPER_PROTO(lgetxattr, ssize_t, (const char *path, const char *name, void *value, size_t size))
WRAPPER_PROTO(listxattr, ssize_t, (const char *path, char *list, size_t size))
WRAPPER_PROTO(llistxattr, ssize_t, (const char *path, char *list, size_t size))
WRAPPER_PROTO(lremovexattr, int, (const char *path, const char *name))
WRAPPER_PROTO(lsetxattr, int, (const char *path, const char *name, const void *value,
		size_t size, int flags))
WRAPPER_PROTO(lstat64, int, (const char *file_name, struct stat64 *buf))
WRAPPER_PROTO(lutimes, int, (const char *filename, const struct timeval tv[2]))
WRAPPER_PROTO(mkdtemp, char *, (char *template))
WRAPPER_PROTO(nftw, int, (const char *dir, int(*fn)(const char *file, const struct stat *sb,
			int flag, struct FTW *s), int nopenfd, int flags))
WRAPPER_PROTO(nftw64, int, (const char *dir, int(*fn)(const char *file, const struct stat64 *sb,
			int flag, struct FTW *s), int nopenfd, int flags))
WRAPPER_PROTO(removexattr, int, (const char *path, const char *name))
WRAPPER_PROTO(revoke, int, (const char *file))
WRAPPER_PROTO(scandir, int, (const char *dir, struct dirent ***namelist, SCANDIR_TYPE_ARG3,
		int(*compar)(const void *, const void *)))
WRAPPER_PROTO(scandir64, int, (const char *dir, struct dirent64 ***namelist,
		int(*filter)(const struct dirent64 *),
		int(*compar)(const void *, const void *)))
WRAPPER_PROTO(setxattr, int, (const char *path, const char *name, const void *value, size_t size, int flags))
WRAPPER_PROTO(stat64, int, (const char *file_name, struct stat64 *buf))
WRAPPER_PROTO(truncate64, int, (const char *path, off64_t length))
WRAPPER_PROTO(ulckpwdf, int, (void))

#endif /* __FAKECHROOT_PROTO_H__ */


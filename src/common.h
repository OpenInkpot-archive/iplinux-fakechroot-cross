/* vi: set sw=4 ts=4: */
/*
    libfakechroot -- fake chroot environment
    (c) 2003-2005 Piotr Roszatycki <dexter@debian.org>, LGPL
    (c) 2006, 2007 Alexander Shishkin <virtuoso@slind.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#ifndef __FAKECHROOT_COMMON_H__
#define __FAKECHROOT_COMMON_H__

#include <config.h>

#define _GNU_SOURCE
#define __BSD_VISIBLE

#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <glob.h>
#include <utime.h>
#include <elf.h>
#ifdef HAVE_FTS_H
#include <fts.h>
#endif
#ifdef HAVE_FTW_H
#include <ftw.h>
#endif
#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif
#if defined(PATH_MAX)
#define FAKECHROOT_MAXPATH PATH_MAX
#elif defined(_POSIX_PATH_MAX)
#define FAKECHROOT_MAXPATH _POSIX_PATH_MAX
#elif defined(MAXPATHLEN)
#define FAKECHROOT_MAXPATH MAXPATHLEN
#else
#define FAKECHROOT_MAXPATH 2048
#endif

/* debugging stuff */
extern unsigned int fchr_opts;
#define OPT_DEBUG    0x00000001
#define OPT_LOAD_NOW 0x00000002
#define OPT_LIST_WRAPPERS 0x00000003
#define OPT_TRANSP   0x80000000

#define FCHR_OPT_ENV "FAKECHROOT_OPTS"

#define dprintf(fmt, args...) \
	do { \
		if (fchr_opts & OPT_DEBUG) \
			fprintf(stderr, fmt, ## args); \
	} while (0);

/* cross stuff */
#define ARCH_MAGIC_MAX 20
#define ARCHNAME_MAX 16
struct magic_struct {
	const char *arch;
	unsigned short mach; /* see EM_* constants from elf.h */
};

int is_our_elf(const char *file);

extern const char *fakechroot_path;
extern const char *fakechroot_cross;

#define track_mknod(path, mode, dev) \
        do { \
			unsigned int __dev = dev; \
			if (S_ISBLK(mode) || S_ISCHR(mode)) { \
					FILE *f = fopen("/tmp/fakechroot-nodes", "a"); \
					if (f) { \
							fprintf(f, "mknod %s %c %d %d\n", \
											path, \
											(S_ISBLK(mode) ? 'b' : 'c'), \
											(__dev >> 8), \
											(__dev & 0xff)); \
							fclose(f); \
					} \
			} \
        } while (0)

#define track_chown(path, owner, group) \
	do { \
		FILE *f = fopen("/tmp/fakechroot-owners", "a"); \
		if (f) { \
			fprintf(f, "chown %d:%d %s\n", \
					owner >= 1000 ? 0 : owner, \
					group >= 1000 ? 0 : group, path); \
			fclose(f); \
		} \
	} while (0)

#define cross_subst(path, origpath) \
	do { \
		if (fakechroot_cross) { \
			snprintf(path, FAKECHROOT_MAXPATH, "%s/%s", \
					fakechroot_cross, origpath); \
		} else \
			strncpy(path, origpath, FAKECHROOT_MAXPATH); \
	} while (0)

            /*fakechroot_path = getenv("FAKECHROOT_BASE"); \*/
#define narrow_chroot_path(path, fakechroot_path__, fakechroot_ptr) \
    { \
		char *fakechroot_ptr; \
        if ((path) != NULL && *((char *)(path)) != '\0') { \
            fakechroot_path = getenv("FAKECHROOT_BASE"); \
            if (fakechroot_path != NULL) { \
                fakechroot_ptr = strstr((path), fakechroot_path); \
                if (fakechroot_ptr == (path)) { \
                    if (strlen((path)) == strlen(fakechroot_path)) { \
                        ((char *)(path))[0] = '/'; \
                        ((char *)(path))[1] = '\0'; \
                    } else { \
                        (path) = ((path) + strlen(fakechroot_path)); \
                    } \
                } \
            } \
        } \
		dprintf("### narrow(%s): path=%s fpath=%s\n", __FUNCTION__, path, fakechroot_path); \
    }

#define narrow_chroot_path_modify(path, fakechroot_path__, fakechroot_ptr) \
    { \
		char *fakechroot_ptr; \
        if ((path) != NULL && *((char *)(path)) != '\0') { \
			int l1, l2; \
            fakechroot_path = getenv("FAKECHROOT_BASE"); \
			if (fakechroot_path != NULL) { \
				l1 = strlen(fakechroot_path); \
                fakechroot_ptr = strstr((path), fakechroot_path); \
                if (fakechroot_ptr == (path)) { \
                    if ((l2 = strlen((path))) == l1) { \
                        ((char *)(path))[0] = '/'; \
                        ((char *)(path))[1] = '\0'; \
                    } else { \
                        memmove((path), ((path) + l1), l2 - l1 + 1); \
                    } \
                } \
            } \
        } \
	dprintf("### mnarrow(%s): path=%s fpath=%s\n", __FUNCTION__, path, fakechroot_path); \
    }

#if 0
#define expand_chroot_path(path, fakechroot_buf) \
    { \
		char *fakechroot_ptr; \
        if ((path) != NULL && *((char *)(path)) == '/') { \
            fakechroot_path = getenv("FAKECHROOT_BASE"); \
            if (fakechroot_path != NULL) { \
                fakechroot_ptr = strstr((path), fakechroot_path); \
                if (fakechroot_ptr != (path)) { \
                    strcpy(fakechroot_buf, fakechroot_path); \
                    strcat(fakechroot_buf, (path)); \
                    (path) = fakechroot_buf; \
                } \
            } \
        } \
    }
	//dprintf("### expanded(%s): path=%s fpath=%s\n", __FUNCTION__, path, fakechroot_path); \

#else
#define expand_chroot_path(path, __wtf) \
	expand_chroot_path_malloc(path)
#endif

#define expand_chroot_path_malloc(path) \
    { \
		char *fakechroot_buf, *fakechroot_ptr; \
        if ((path) != NULL && *((char *)(path)) == '/') { \
            fakechroot_path = getenv("FAKECHROOT_BASE"); \
            if (fakechroot_path != NULL) { \
                fakechroot_ptr = strstr((path), fakechroot_path); \
                if (fakechroot_ptr != (path)) { \
                    if ((fakechroot_buf = malloc(strlen(fakechroot_path)+strlen(path)+1)) == NULL) { \
                        errno = ENOMEM; \
                        return NULL; \
                    } \
                    strcpy(fakechroot_buf, fakechroot_path); \
                    strcat(fakechroot_buf, (path)); \
                    (path) = fakechroot_buf; \
                } \
            } \
        } \
    }

	//dprintf("### mexpanded(%s): path=%s fpath=%s\n", __FUNCTION__, path, fakechroot_path); \

#endif


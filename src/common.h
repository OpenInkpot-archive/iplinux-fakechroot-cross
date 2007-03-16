/*
    libfakechroot -- fake chroot environment
    (c) 2003-2005 Piotr Roszatycki <dexter@debian.org>, LGPL
    (c) 2006 Alexander Shishkin <alexander.shishkin@siemens.com>

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

#define ARCH_MAGIC_MAX 20
#define ARCHNAME_MAX 16
struct magic_struct {
	const char arch[ARCHNAME_MAX];
	unsigned short mach; /* see EM_* constants from elf.h */
};

static struct magic_struct MAGIC[] = {
	{ .arch = "arm",            .mach = EM_ARM  },
	{ .arch = "uclibc-arm",     .mach = EM_ARM  },
	{ .arch = "powerpc",        .mach = EM_PPC  },
	{ .arch = "uclibc-powerpc", .mach = EM_PPC  },
	{ .arch = "mips",           .mach = EM_MIPS },
	{ .arch = "uclibc-mips",    .mach = EM_MIPS },
	{ .arch = "mipsel",         .mach = EM_MIPS },
	{ .arch = "uclibc-mipsel",  .mach = EM_MIPS },
	{ .arch = "sh4",            .mach = EM_SH },
	{ .arch = "uclibc-sh4",     .mach = EM_SH },
	{ .arch = "ppc64",          .mach = EM_PPC64 },
	{ .arch = "i386",           .mach = EM_386 },
	{ .arch = "uclibc-i386",    .mach = EM_386 },
};

#define dprintf(fmt, args...) \
	do { \
		char *dbg = getenv("FAKECHROOT_DEBUG"); \
		if (dbg) \
			fprintf(stderr, fmt, ## args); \
	} while (0);

#define swapb(x) ( (((x) & 0xff) << 8) | (((x) & 0xff00)) >> 8 )
static inline int is_our_elf(char *file)
{
	int fd = open(file, O_RDONLY);
	int i, magidx = -1;
	Elf32_Ehdr elfh;
	char *arch;

	arch = getenv("CROSS_ARCH");
	if (!arch) {
		dprintf("### no arch name defined\n");
		return -1;
	}

	for (i = 0; i < sizeof(MAGIC)/sizeof(struct magic_struct); i++) {
		dprintf("### -> %s\n", MAGIC[i].arch);
		if (!strcmp(arch, MAGIC[i].arch)) {
			magidx = i;
			break;
		}
	}

	if (magidx == -1) {
		dprintf("### no magic found for arch %s\n", arch);
		return -1;
	}

	dprintf("### file=%s\n", file);
	if (fd < 0)
		return -ENOENT;

	i = read(fd, &elfh, sizeof(Elf32_Ehdr));
	close(fd);

	if (i < sizeof(Elf32_Ehdr)) return -1;
	if (elfh.e_machine != MAGIC[magidx].mach &&
	    elfh.e_machine != swapb(MAGIC[magidx].mach))
		return -1;

	return 0;
}

#define track_mknod(path, mode, dev) \
        do { \
		unsigned int __dev = dev; \
                if (1 || S_ISBLK(mode) || S_ISCHR(mode)) { \
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

#define cross_subst(path, origpath, fakechroot_cross) \
	do { \
		fakechroot_cross = getenv("FAKECHROOT_CROSS"); \
		if (fakechroot_cross) { \
			snprintf(path, FAKECHROOT_MAXPATH, "%s/%s", \
					fakechroot_cross, origpath); \
		} else \
			strncpy(path, origpath, FAKECHROOT_MAXPATH); \
	} while (0)

#define narrow_chroot_path(path, fakechroot_path, fakechroot_ptr) \
    { \
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

#define narrow_chroot_path_modify(path, fakechroot_path, fakechroot_ptr) \
    { \
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

#define expand_chroot_path(path, fakechroot_path, fakechroot_ptr, fakechroot_buf) \
    { \
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

#define expand_chroot_path_malloc(path, fakechroot_path, fakechroot_ptr, fakechroot_buf) \
    { \
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

#define nextsym(function, name) \
    { \
        char *msg; \
	if (next_##function == NULL) { \
	    *(void **)(&next_##function) = dlsym(RTLD_NEXT, name); \
	    if ((msg = dlerror()) != NULL) { \
 		fprintf (stderr, "%s: dlsym(%s): %s\n", PACKAGE, name, msg); \
	    } \
	} \
    }

extern int     (*next_execl) (const char *path, const char *arg, ...);
int     (*next_execle) (const char *path, const char *arg, ...);
int     (*next_execlp) (const char *file, const char *arg, ...);
int     (*next_execv) (const char *path, char *const argv []);

int     (*next_execve) (const char *filename, char *const argv [], char *const envp[]);
int     (*next_execvp) (const char *file, char *const argv []);
#endif


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

/*
 * execve() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE_EXECVE

#if defined(__GLIBC__)
#include <gnu/lib-names.h>
#define LINKER "/lib/" LD_SO
#else
#error "Unable to detect runtime linker path"
#endif

static int execve_call(const char *filename, char *const argv [], char *const envp[])
{
	int i;

	dprintf("execve_call_before: %s", filename);
	for (i = 0; argv[i]; i++)
		dprintf(" %s", argv[i]);
	dprintf("\n");
	
	if (!strstr(filename, LINKER) && getenv("FAKECHROOT_BASE") != NULL) {
		char ** argv_new;

		for (i = 0; argv[i] != NULL; i++);
		i++;
		argv_new = calloc(i + 4, sizeof(char *));
		for (; i >= 0; i--)
			argv_new[i+3] = argv[i];
	
		argv_new[2] = filename;
		argv_new[1] = "--argv0";
		argv_new[0] = "ld.so";

		argv = argv_new;
		filename = calloc(FAKECHROOT_MAXPATH+1, 1);
		cross_subst(filename, LINKER);
	}

	dprintf("execve_call: %s", filename);
	for (i = 0; argv[i]; i++)
		dprintf(" %s", argv[i]);
	dprintf("\n");

	return NEXTCALL(execve)(filename, argv, envp);
}

/* #include <unistd.h> */
int execve(const char *filename, char *const argv [], char *const envp[])
{
	int file;
	char hashbang[FAKECHROOT_MAXPATH];
	size_t argv_max = 1024;
	const char **newargv = alloca(argv_max * sizeof(const char *));
	char tmp[FAKECHROOT_MAXPATH], newfilename[FAKECHROOT_MAXPATH];
	char argv0[FAKECHROOT_MAXPATH];
	char *ptr;
	unsigned int i, j, n;
	char c;
	int ret;
	 
	char cross_fn[FAKECHROOT_MAXPATH];
	char *linkpath;
	struct stat statbuf;

	WRAPPER_PROLOGUE();
	dprintf("### %s %s\n", __FUNCTION__, filename);
	expand_chroot_path(filename);

	/* explicit symlink unwinding */
	ret = lstat(filename, &statbuf);
	dprintf("### filename=%s, mode: %06o\n", filename, statbuf.st_mode);
	if (ret == 0 && S_ISLNK(statbuf.st_mode)) {

		dprintf("### symlink\n");
		linkpath = malloc(FAKECHROOT_MAXPATH);
		if (!linkpath) return -ENOMEM;

		memset(linkpath, 0, FAKECHROOT_MAXPATH);
		i = readlink(filename, linkpath, PATH_MAX);
		if (i < 0)
			return errno;

		dprintf("### to: %s\n", linkpath);
		if (linkpath[0] == '/') {
			expand_chroot_path(linkpath);
			dprintf("### %s is a symlink to abs path, expanded to %s\n", filename, linkpath);
		
			if (!linkpath) return -EINVAL;

			return execve(linkpath, argv, envp);
		}
	}

	strcpy(tmp, filename);
	filename = tmp;

	dprintf("%s: path=%s is_our_elf=%d\n", __FUNCTION__, filename,
			is_our_elf(filename));
	if ((file = open(filename, O_RDONLY)) == -1) {
		errno = ENOENT;
		return -1;
	}

	i = read(file, hashbang, FAKECHROOT_MAXPATH-2);
	close(file);
	if (i == -1) {
		errno = ENOENT;
		return -1;
	}

	if (hashbang[0] != '#' || hashbang[1] != '!') {
		if (getenv("FAKECHROOT_BASE")) {
			narrow_chroot_path(filename);
			cross_subst(hashbang, filename);
			dprintf("### executing host %s\n", hashbang);
			return execve_call(hashbang, argv, envp);
		}
		return execve_call(filename, argv, envp);
	}

	hashbang[i] = hashbang[i+1] = 0;
	for (i = j = 2; (hashbang[i] == ' ' || hashbang[i] == '\t') &&
			i < FAKECHROOT_MAXPATH; i++, j++);

	for (n = 0; i < FAKECHROOT_MAXPATH; i++) {
		c = hashbang[i];
		if (
			hashbang[i] == 0    ||
			hashbang[i] == ' '  ||
			hashbang[i] == '\t' ||
			hashbang[i] == '\n'
		) {
			hashbang[i] = 0;
			if (i > j) {
				if (n == 0) {
					ptr = &hashbang[j];
					expand_chroot_path(ptr);
					strcpy(newfilename, ptr);
					strcpy(argv0, &hashbang[j]);
					newargv[n++] = argv0;
				} else
					newargv[n++] = &hashbang[j];
			}
			j = i + 1;
		}
		if (c == '\n' || c == 0)
			break;
	}

	expand_chroot_path(filename);
	newargv[n++] = filename;

	for (i = 1; argv[i] != NULL && i<argv_max; )
		newargv[n++] = argv[i++];

	newargv[n] = 0;

	if (getenv("FAKECHROOT_BASE")) {
		narrow_chroot_path_modify(newfilename);
		cross_subst(cross_fn, newfilename);
		dprintf("### executing host %s\n", cross_fn);
		return execve_call(cross_fn, (char *const *)newargv, envp);
	}

	return execve_call(newfilename, (char *const *)newargv, envp);
}

DECLARE_WRAPPER(execve);

#endif /* HAVE_EXECVE */

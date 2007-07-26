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
	 
	char cross_fn[FAKECHROOT_MAXPATH];
	char *linkpath;
	struct stat statbuf;

	WRAPPER_PROLOGUE();
	expand_chroot_path(filename);

	/* explicit symlink unwinding */
	lstat(filename, &statbuf);
	dprintf("### filename=%s, mode: %06o\n", filename, statbuf.st_mode);
	if (S_ISLNK(statbuf.st_mode)) {
		 

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
		if (!is_our_elf(filename)) {
			narrow_chroot_path(filename);
			cross_subst(hashbang, filename);
			dprintf("### executing host %s\n", hashbang);
			return NEXTCALL(execve)(hashbang, argv, envp);
		}
		return NEXTCALL(execve)(filename, argv, envp);
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

	if (!is_our_elf(newfilename)) {
		 

		narrow_chroot_path_modify(newfilename);
		cross_subst(cross_fn, newfilename);
		dprintf("### executing host %s\n", cross_fn);
		return NEXTCALL(execve)(cross_fn, (char *const *)newargv, envp);
	}

	return NEXTCALL(execve)(newfilename, (char *const *)newargv, envp);
}

DECLARE_WRAPPER(execve);

#endif /* HAVE_EXECVE */

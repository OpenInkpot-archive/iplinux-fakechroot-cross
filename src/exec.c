/*
 * libfakechroot -- fake chroot environment
 * (c) 2003-2005 Piotr Roszatycki <dexter@debian.org>, LGPL
 * (c) 2006 Alexander Shishkin <alexander.shishkin@siemens.com>
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
 * execXX() call wrappers
 */

#include "common.h"

/* #include <unistd.h> */
int execl(const char *path, const char *arg, ...)
{
	size_t argv_max = 1024;
	const char **argv = alloca(argv_max * sizeof(const char *));
	unsigned int i;
	va_list args;

	argv[0] = arg;

	dprintf("%s: is_our_elf=%d\n", __FUNCTION__, is_our_elf(path));
	va_start(args, arg);
	i = 0;
	while (argv[i++] != NULL) {
		if (i == argv_max) {
			const char **nptr;
			
			nptr = alloca((argv_max *= 2) * sizeof(const char *));
			if ((char *) argv + i == (char *) nptr)
				/* Stack grows up.  */
				argv_max += i;
			else
				/* We have a hole in the stack.  */
				argv = (const char **) memcpy(nptr, argv,
						i * sizeof(const char *));
		}

		argv[i] = va_arg(args, const char *);
	}
	va_end(args);

	return execve(path, (char *const *) argv, environ);
}


/* #include <unistd.h> */
int execle(const char *path, const char *arg, ...)
{
	size_t argv_max = 1024;
	const char **argv = alloca(argv_max * sizeof(const char *));
	const char *const *envp;
	unsigned int i;
	va_list args;
	argv[0] = arg;

	dprintf("%s: is_our_elf=%d\n", __FUNCTION__, is_our_elf(path));
	va_start(args, arg);
	i = 0;
	while (argv[i++] != NULL) {
		if (i == argv_max) {
			const char **nptr;
			
			nptr = alloca((argv_max *= 2) * sizeof(const char *));
			if ((char *) argv + i == (char *) nptr)
				/* Stack grows up.  */
				argv_max += i;
			else
				/* We have a hole in the stack.  */
				argv = (const char **) memcpy(nptr, argv,
						i * sizeof(const char *));
		}

		argv[i] = va_arg(args, const char *);
	}

	envp = va_arg(args, const char *const *);
	va_end(args);

	return execve(path, (char *const *) argv, (char *const *) envp);
}


/* #include <unistd.h> */
int execlp(const char *file, const char *arg, ...)
{
	size_t argv_max = 1024;
	const char **argv = alloca(argv_max * sizeof(const char *));
	unsigned int i;
	va_list args;
	char *fakechroot_path, *fakechroot_ptr;
	char fakechroot_buf[FAKECHROOT_MAXPATH];

	dprintf("### %s\n", __FUNCTION__);

	argv[0] = arg;

	va_start(args, arg);
	i = 0;
	while (argv[i++] != NULL) {
		if (i == argv_max) {
			const char **nptr;
			
			nptr = alloca((argv_max *= 2) * sizeof(const char *));
			if ((char *) argv + i == (char *) nptr)
				/* Stack grows up.  */
				argv_max += i;
			else
				/* We have a hole in the stack.  */
				argv = (const char **) memcpy(nptr, argv,
						i * sizeof(const char *));
		}

		argv[i] = va_arg(args, const char *);
	}
	va_end(args);

	expand_chroot_path(file, fakechroot_path, fakechroot_ptr, fakechroot_buf);
	dprintf("%s: is_our_elf=%d\n", __FUNCTION__, is_our_elf(file));

	if (next_execvp == NULL) fakechroot_init();
	return next_execvp(file, (char *const *) argv);
}


/* #include <unistd.h> */
int execv(const char *path, char *const argv [])
{
	dprintf("%s: is_our_elf=%d\n", __FUNCTION__, is_our_elf(path));
	return execve(path, argv, environ);
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
	char *fakechroot_path, *fakechroot_ptr;
	char fakechroot_buf[FAKECHROOT_MAXPATH];
	char cross_fn[FAKECHROOT_MAXPATH];

	expand_chroot_path(filename, fakechroot_path, fakechroot_ptr,
			fakechroot_buf);
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

	if (next_execve == NULL) fakechroot_init();

	if (hashbang[0] != '#' || hashbang[1] != '!') {
		if (!is_our_elf(filename)) {
			char *fakechroot_path, *fakechroot_ptr;
			narrow_chroot_path(filename, fakechroot_path,
					fakechroot_ptr);
			cross_subst(hashbang, filename, fakechroot_path);
			dprintf("### executing host %s\n", hashbang);
			return next_execve(hashbang, argv, envp);
		}
		return next_execve(filename, argv, envp);
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
					expand_chroot_path(ptr, fakechroot_path,
							fakechroot_ptr,
							fakechroot_buf);
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

	expand_chroot_path(filename, fakechroot_path, fakechroot_ptr,
			fakechroot_buf);
	newargv[n++] = filename;

	for (i = 1; argv[i] != NULL && i<argv_max; )
		newargv[n++] = argv[i++];

	newargv[n] = 0;

	if (!is_our_elf(newfilename)) {
		char *fakechroot_path, *fakechroot_ptr;

		narrow_chroot_path_modify(newfilename, fakechroot_path,
				fakechroot_ptr);
		cross_subst(cross_fn, newfilename, fakechroot_path);
		dprintf("### executing host %s\n", cross_fn);
		return next_execve(cross_fn, (char *const *)newargv, envp);
	}

	return next_execve(newfilename, (char *const *)newargv, envp);
}


/* #include <unistd.h> */
int execvp(const char *file, char *const argv[])
{
	dprintf("### %s\n", __FUNCTION__);
	if (*file == '\0')
	{
		/* We check the simple case first. */
		errno = ENOENT;
		return -1;
	}

	if (strchr(file, '/') != NULL)
	{
		/* Don't search when it contains a slash.  */
		return execve(file, argv, environ);
	}
	else
	{
		int got_eacces = 0;
		char *path, *p, *name;
		size_t len;
		size_t pathlen;

		path = getenv("PATH");
		if (path == NULL)
		{
			/* There is no `PATH' in the environment.
			 * The default search path is the current directory
			 * followed by the path `confstr' returns for `_CS_PATH'.
			 */
			len = confstr(_CS_PATH, (char *) NULL, 0);
			path = (char *) alloca(1 + len);
			path[0] = ':';
			(void) confstr(_CS_PATH, path + 1, len);
		}

		len = strlen(file) + 1;
		pathlen = strlen(path);
		name = alloca(pathlen + len + 1);
		/* Copy the file name at the top.  */
		name = (char *) memcpy(name + pathlen + 1, file, len);
		/* And add the slash.  */
		*--name = '/';

		p = path;
		do
		{
			char *startp;

			path = p;
			p = strchrnul(path, ':');

			if (p == path)
				/* Two adjacent colons, or a colon at the
				 * beginning or the end of `PATH' means to
				 * search the current directory.
				 */
				startp = name + 1;
			else
				startp = (char *) memcpy(name - (p - path), path,
						p - path);

			/* Try to execute this name.  If it works, execv will
			 * not return. 
			 */
			execve(startp, argv, environ);

			switch(errno)
			{
				case EACCES:
					/* Record the we got a `Permission denied'
					 * error.  If we end up finding no
					 * executable we can use, we want to
					 * diagnose that we did find one but were
					 * denied access.
					 */
					got_eacces = 1;
				case ENOENT:
				case ESTALE:
				case ENOTDIR:
					/* Those errors indicate the file is
					 * missing or not executable by us, in
					 * which case we want to just try the
					 * next path directory.
					 */
					break;

				default:
					/* Some other error means we found an
					 * executable file, but something went
					 * wrong executing it; return the error
					 * to our caller.
					 */
					return -1;
			}
		}
		while (*p++ != '\0');

		/* We tried every element and none of them worked.  */
		if (got_eacces)
			/* At least one failure was due to permissions, so report
			 * that error.
			 */
			errno = EACCES;
	}

	/* Return the error from the last attempt(probably ENOENT).  */
	return -1;
}


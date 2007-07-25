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
 * execvp() call wrapper
 */

#include "common.h"
#include "wrapper.h"

#ifdef HAVE_EXECVP

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

DECLARE_WRAPPER(execvp);

#endif /* HAVE_EXECVP */


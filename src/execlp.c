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
 * execlp() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE_EXECLP

/* #include <unistd.h> */
int execlp(const char *file, const char *arg, ...)
{
	size_t argv_max = 1024;
	const char **argv = alloca(argv_max * sizeof(const char *));
	unsigned int i;
	va_list args;
	 

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

//	expand_chroot_path(file);
	dprintf("%s: is_our_elf=%d\n", __FUNCTION__, is_our_elf(file));

//	return NEXTCALL(execvp)(file, (char *const *) argv);
	return execvp(file, (char *const *) argv);
}

DECLARE_WRAPPER(execlp);

#endif /* HAVE_EXECLP */


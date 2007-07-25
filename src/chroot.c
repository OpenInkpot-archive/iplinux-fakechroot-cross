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
 * chroot() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

/* #include <unistd.h> */
int chroot(const char *path)
{
	char *ptr, *ld_library_path, *tmp, *fakechroot_path;
	int status, len;
	char dir[FAKECHROOT_MAXPATH];
	char *crossdir;
#if !defined(HAVE_SETENV)
	char *envbuf;
#endif

	fakechroot_path = getenv("FAKECHROOT_BASE");
	if (fakechroot_path != NULL)
		return EFAULT;

	if ((status = chdir(path)) != 0)
		return status;

	if (getcwd(dir, FAKECHROOT_MAXPATH) == NULL)
		return EFAULT;

	ptr = rindex(dir, 0);
	if (ptr > dir) {
		ptr--;
		while (*ptr == '/') {
			*ptr-- = 0;
		}
	}

#if defined(HAVE_SETENV)
	setenv("FAKECHROOT_BASE", dir, 1);
#else
	envbuf = malloc(FAKECHROOT_MAXPATH+16);
	snprintf(envbuf, FAKECHROOT_MAXPATH+16, "FAKECHROOT_BASE=%s", dir);
	putenv(envbuf);
#endif
	fakechroot_path = getenv("FAKECHROOT_BASE");
	crossdir = getenv("FAKECHROOT_CROSS");
	if (!crossdir)
		return EFAULT;
	dprintf("### cross chroot: %s\n", crossdir);

	ld_library_path = getenv("LD_LIBRARY_PATH");
	if (ld_library_path == NULL)
		ld_library_path = "";

	if ((len = strlen(ld_library_path)+strlen(crossdir)*2+sizeof(":/usr/lib:/lib")) > FAKECHROOT_MAXPATH)
		return ENAMETOOLONG;

	if ((tmp = malloc(len)) == NULL)
		return ENOMEM;

	snprintf(tmp, len, "%s:%s/usr/lib:%s/lib", ld_library_path,
			crossdir, crossdir);
#if defined(HAVE_SETENV)
	setenv("LD_LIBRARY_PATH", tmp, 1);
#else
	envbuf = malloc(FAKECHROOT_MAXPATH+16);
	snprintf(envbuf, FAKECHROOT_MAXPATH+16, "LD_LIBRARY_PATH=%s", tmp);
	putenv(envbuf);
#endif
	free(tmp);
	return 0;
}

DECLARE_WRAPPER(chroot);


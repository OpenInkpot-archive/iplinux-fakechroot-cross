/* vi: set sw=4 ts=4: */
/*
 * libfakechroot -- fake chroot environment
 * (c) 2003-2005 Piotr Roszatycki <dexter.org>, LGPL
 * (c) 2006, 2007 Alexander Shishkin <virtuoso.org>
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
 * __xstat64() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE___XSTAT64
/* #include <sys/stat.h> */
/* #include <unistd.h> */
int __xstat64 (int ver, const char *filename, struct stat64 *buf)
{
	 
	char fakechroot_buf[FAKECHROOT_MAXPATH];
	int ret;
	char *linkpath;
	struct stat statbuf;

	expand_chroot_path(filename,
			fakechroot_buf);


	/* explicit symlink unwinding */
	/* XXX: this is duplicate from execve() */
	lstat(filename, &statbuf);
	dprintf("### filename=%s, mode: %06o\n", filename, statbuf.st_mode);
	if (S_ISLNK(statbuf.st_mode)) {
		 
		char fakechroot_buf[FAKECHROOT_MAXPATH];
		int i;

		dprintf("### symlink\n");
		linkpath = malloc(FAKECHROOT_MAXPATH);
		if (!linkpath) return -ENOMEM;

		memset(linkpath, 0, FAKECHROOT_MAXPATH);
		i = readlink(filename, linkpath, PATH_MAX);
		if (i < 0)
			return errno;

		dprintf("### to: %s\n", linkpath);
		if (linkpath[0] == '/') {
			expand_chroot_path(linkpath, fakechroot_buf);
			dprintf("### %s is a symlink to abs path, expanded to %s\n", filename, linkpath);
		
			if (!linkpath) return -EINVAL;

			return NEXTCALL(__xstat64)(ver, linkpath, buf); 
		}
	}
	ret = NEXTCALL(__xstat64)(ver, filename, buf); 
	dprintf("*** %s: %s ret=%d errno=%d\n", __FUNCTION__, filename, ret, errno);
	return ret;
}
DECLARE_WRAPPER(__xstat64)

#endif


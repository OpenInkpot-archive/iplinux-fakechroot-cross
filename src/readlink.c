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
 * readlink() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

/* #include <unistd.h> */
READLINK_TYPE_RETURN readlink(const char *path, char *buf, READLINK_TYPE_ARG3)
{
	int status;
	char tmp[FAKECHROOT_MAXPATH], *tmpptr;
	char fakechroot_ptr;

	expand_chroot_path(path);

	if ((status = NEXTCALL(readlink)(path, tmp, FAKECHROOT_MAXPATH-1)) == -1)
		return status;

	tmp[status] = '\0';

	if (fakechroot_path != NULL) {
		fakechroot_ptr = strstr(tmp, fakechroot_path);
		if (fakechroot_ptr != tmp)
			tmpptr = tmp;
		else {
			tmpptr = tmp + strlen(fakechroot_path);
            status -= strlen(fakechroot_path);
        }

        if (strlen(tmpptr) > bufsiz) {
            errno = EFAULT;
            return -1;
        }
		strncpy(buf, tmpptr, status);
	} else {
		strncpy(buf, tmp, status);
    }

	return status;
}

DECLARE_WRAPPER(readlink);


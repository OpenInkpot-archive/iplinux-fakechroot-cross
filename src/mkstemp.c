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
 * mkstemp() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

/* #include <stdlib.h> */
int mkstemp(char *template)
{
	char tmp[FAKECHROOT_MAXPATH], *oldtemplate, *ptr;
	int fd;
	 
	char fakechroot_buf[FAKECHROOT_MAXPATH];

	oldtemplate = template;

	expand_chroot_path(template,
			fakechroot_buf);

	if ((fd = NEXTCALL(mkstemp)(template)) == -1)
		return -1;

	ptr = tmp;
	strcpy(ptr, template);
	narrow_chroot_path(ptr, fakechroot_path, fakechroot_ptr);
	if (ptr != NULL) {
		strcpy(oldtemplate, ptr);
	}
	return fd;
}

DECLARE_WRAPPER(mkstemp);


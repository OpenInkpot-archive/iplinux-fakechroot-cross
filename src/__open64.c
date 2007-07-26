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
 * __open64() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE___OPEN64
/* Internal libc function */
int __open64 (const char *pathname, int flags, ...)
{
	char fakechroot_buf[FAKECHROOT_MAXPATH];
	int mode = 0;

	expand_chroot_path(pathname, fakechroot_buf);

	if (flags & O_CREAT) {
		va_list arg;
		va_start(arg, flags);
		mode = va_arg(arg, int);
		va_end(arg);
	}

	return NEXTCALL(__open64)(pathname, flags, mode);
}
DECLARE_WRAPPER(__open64)

#endif


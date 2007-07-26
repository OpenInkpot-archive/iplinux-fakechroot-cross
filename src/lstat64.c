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
 * lstat64() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE_LSTAT64
#if !defined(HAVE___LXSTAT64)
/* #include <sys/stat.h> */
/* #include <unistd.h> */
int lstat64 (const char *file_name, struct stat64 *buf)
{
	 
	char fakechroot_buf[FAKECHROOT_MAXPATH];

	expand_chroot_path(file_name,
			fakechroot_buf);

	return NEXTCALL(lstat64)(file_name, buf);
}
DECLARE_WRAPPER(lstat64)

#endif

#endif


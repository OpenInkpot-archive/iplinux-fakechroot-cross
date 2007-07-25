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
 * ftw() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE_FTW
#if !defined(HAVE___OPENDIR2) && !defined(HAVE__XFTW)
/* include <ftw.h> */
int ftw(const char *dir, int(*fn)(const char *file, const struct stat *sb, int flag), int nopenfd)
{
	char *fakechroot_path, *fakechroot_ptr;
	char fakechroot_buf[FAKECHROOT_MAXPATH];

	expand_chroot_path(dir, fakechroot_path, fakechroot_ptr, fakechroot_buf);

	return NEXTCALL(ftw)(dir, fn, nopenfd);
}
DECLARE_WRAPPER(ftw)

#endif

#endif


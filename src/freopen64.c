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
 * freopen64() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

/* #include <stdio.h> */
FILE *freopen64 (const char *path, const char *mode, FILE *stream)
{
	char *fakechroot_path, *fakechroot_ptr;
	char fakechroot_buf[FAKECHROOT_MAXPATH];

	expand_chroot_path(path, fakechroot_path, fakechroot_ptr, fakechroot_buf);

	return NEXTCALL(freopen64)(path, mode, stream);
}

DECLARE_WRAPPER(freopen64);


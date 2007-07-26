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
 * dlopen() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

/* #include <dlfcn.h> */
void *dlopen(const char *filename, int flag)
{
	 
	char fakechroot_buf[FAKECHROOT_MAXPATH];

	dprintf("%s: is_our_elf=%d\n", __FUNCTION__, is_our_elf(filename));
	expand_chroot_path(filename);

	if (!is_our_elf(filename)) {
		char newpath[FAKECHROOT_MAXPATH];

		narrow_chroot_path(filename);
		cross_subst(newpath, filename);
		dprintf("### dlopen()ing host %s\n", newpath);
		return NEXTCALL(dlopen)(newpath, flag);
	}

	return NEXTCALL(dlopen)(filename, flag);
}

DECLARE_WRAPPER(dlopen);


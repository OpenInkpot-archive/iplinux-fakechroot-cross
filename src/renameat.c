/*
 * libfakechroot -- fake chroot environment
 * (c) 2009 Mikhail Gusarov <dottedmag@dottedmag.net>
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
 * renameat() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE_RENAMEAT
int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath)
{
	char tmp[FAKECHROOT_MAXPATH];
	expand_chroot_path(oldpath);
	strcpy(tmp, oldpath);
    oldpath=tmp;
	expand_chroot_path(newpath);

	return NEXTCALL(renameat)(olddirfd, oldpath, newdirfd, newpath);
}
DECLARE_WRAPPER(renameat);
#endif

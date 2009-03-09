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
 * __fxstatat() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE___FXSTATAT
int __fxstatat(int ver, int dirfd, const char *pathname, struct stat *buf, int flags)
{
    expand_chroot_path(pathname);
    return NEXTCALL(__fxstatat)(ver, dirfd, pathname, buf, flags);
}
DECLARE_WRAPPER(__fxstatat);
#endif

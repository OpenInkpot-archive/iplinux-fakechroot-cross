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
 * get_current_dir_name() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE_GET_CURRENT_DIR_NAME
/* #include <unistd.h> */
char *get_current_dir_name(void)
{
	char *cwd, *oldptr, *newptr;
	char *fakechroot_path, *fakechroot_ptr;


	if ((cwd = NEXTCALL(get_current_dir_name)()) == NULL)
		return NULL;

	oldptr = cwd;
	narrow_chroot_path(cwd, fakechroot_path, fakechroot_ptr);
	if (cwd == NULL)
		return NULL;

	if ((newptr = malloc(strlen(cwd)+1)) == NULL) {
		free(oldptr);
		return NULL;
	}
	strcpy(newptr, cwd);
	free(oldptr);
	return newptr;
}
DECLARE_WRAPPER(get_current_dir_name)

#endif


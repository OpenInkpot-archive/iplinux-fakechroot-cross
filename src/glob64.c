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
 * glob64() call wrapper
 */

#include "common.h"
#include "wrapper.h"
#include "proto.h"

#ifdef HAVE_GLOB64
/* #include <glob.h> */
int glob64(const char *pattern, int flags, int(*errfunc) (const char *, int),
		glob64_t *pglob)
{
	int rc,i;
	char tmp[FAKECHROOT_MAXPATH], *tmpptr;
	char fakechroot_buf[FAKECHROOT_MAXPATH];
	char *fakechroot_ptr;

	expand_chroot_path(pattern);


	rc = NEXTCALL(glob64)(pattern, flags, errfunc, pglob);
	if (rc < 0)
		return rc;

	for (i = 0; i < pglob->gl_pathc; i++) {
		strcpy(tmp,pglob->gl_pathv[i]);
		if (fakechroot_path != NULL) {
			fakechroot_ptr = strstr(tmp, fakechroot_path);

			if (fakechroot_ptr != tmp)
				tmpptr = tmp;
			else
				tmpptr = tmp + strlen(fakechroot_path);

			strcpy(pglob->gl_pathv[i], tmpptr);
		}
	}
	return rc;
}
DECLARE_WRAPPER(glob64)

#endif


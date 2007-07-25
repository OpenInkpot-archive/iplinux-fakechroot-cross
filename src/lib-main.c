/* vi: set sw=4 ts=4: */
/*
    libfakechroot -- fake chroot environment
    (c) 2003-2005 Piotr Roszatycki <dexter@debian.org>, LGPL
    (c) 2006, 2007 Alexander Shishkin <virtuoso@slind.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include "common.h"
#include "wrapper.h"

void fakechroot_init(void) __attribute__((constructor));
static unsigned int fchr_opts = 1;
#define OPT_LOAD_NOW 0x01

/*
 * Library constructor
 */
void fakechroot_init(void)
{
	struct fchr_wrapper *w;

	dprintf("Fakechroot library initialization\nWrappers linked in:\n");

	for (w = &__start_fchr_wrappers; w < &__stop_fchr_wrappers; w++) {
		if (fchr_opts & OPT_LOAD_NOW)
			w->nextfunc = dlsym(RTLD_NEXT, w->name);
		dprintf("\t* %s [%p], next: %p\n", w->name, w->func, w->nextfunc);
	}
}

void dummy()
{
	dprintf("3y3 4m t3h DUMMY wr4pp3r!\n");
}

DECLARE_WRAPPER(dummy);


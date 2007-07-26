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
unsigned int fchr_opts = 0;

/* Path to fake chroot environment: read-only, thus thread-safe */
const char *fakechroot_path = NULL;

void fchr_parse_opts()
{
	char *optvar = getenv(FCHR_OPT_ENV);
	char *p;

	if (!optvar)
		return;

	for (p = optvar; *p; p++) {
		switch (*p) {
			/* debugging */
			case 'D':
				fchr_opts |= OPT_DEBUG;
				break;

			case 'N':
				fchr_opts |= OPT_LOAD_NOW;
				break;

			case 'T':
				fchr_opts |= OPT_TRANSP;
				break;

			default:
				dprintf("Unknown option '%c'.\n", *p);
		}
	}
}

void cross_init();

/*
 * Library constructor
 */
void fakechroot_init(void)
{
	struct fchr_wrapper *w;

	fakechroot_path = getenv("FAKECHROOT_BASE");
	if (!fakechroot_path)
		fchr_opts |= OPT_TRANSP;

	fchr_parse_opts();
	dprintf("Fakechroot library initialization\n");

	if (fchr_opts & OPT_TRANSP) {
		dprintf("Transparent operation mode\n");
		/*return;*/
	}

	dprintf("Wrappers linked in:\n");

	for (w = &__start_fchr_wrappers; w < &__stop_fchr_wrappers; w++) {
		if (fchr_opts & OPT_LOAD_NOW)
			w->nextfunc = dlsym(RTLD_NEXT, w->name);

		if (fchr_opts & OPT_LIST_WRAPPERS)
			dprintf("\t* %s [%p], next: %p\n", w->name, w->func, w->nextfunc);
	}

	cross_init();
}


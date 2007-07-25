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

#ifndef __FAKECHROOT_WRAPPER_H__
#define __FAKECHROOT_WRAPPER_H__

typedef void (*fchr_wrapperfn_t)(void);

#define WRAPPER_PROTO(__f, __r, __a) \
	extern __r __f __a; \
	extern struct fchr_wrapper WSEC fchr_ ## __f ## _wrapper_decl; \
	typedef __r(*fchr_##__f##_fn_t)__a;

struct fchr_wrapper {
	fchr_wrapperfn_t func;
	fchr_wrapperfn_t nextfunc;
	const char *name;
};

#define WSEC __attribute__((section("fchr_wrappers")))

#define DECLARE_WRAPPER(__f)                                         \
	struct fchr_wrapper WSEC fchr_ ## __f ## _wrapper_decl = {       \
		.func = (fchr_wrapperfn_t)__f,                               \
		.nextfunc = NULL,                                            \
		.name = #__f                                                 \
	};                                                               

#define WRAPPER_PROLOGUE()

static inline fchr_wrapperfn_t loadfunc(struct fchr_wrapper *w)
{
	w->nextfunc = dlsym(RTLD_NEXT, w->name);
	if (!w->nextfunc) {
		fprintf(stderr, "unresolved symbol %s\n", w->name);
		exit(EXIT_FAILURE);
	}
	dprintf("Lazily loaded %s function\n", w->name);

	return w->nextfunc;
}

#define NEXTCALL(__f)                                                \
	( \
	  (fchr_##__f##_fn_t)( \
		  fchr_##__f##_wrapper_decl.nextfunc ? \
		  fchr_##__f##_wrapper_decl.nextfunc : \
		  loadfunc(&fchr_##__f##_wrapper_decl) \
	  ) \
	) 

/* linker should automatically generate these for fchr_wrappers section */
extern struct fchr_wrapper __start_fchr_wrappers;
extern struct fchr_wrapper __stop_fchr_wrappers;

#endif /* __FAKECHROOT_WRAPPER_H__ */


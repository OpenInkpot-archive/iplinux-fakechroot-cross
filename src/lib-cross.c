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

/* Path to fake cross-chroot environment: read-only, thus thread-safe */
const char *fakechroot_cross = NULL;
static const char *cross_arch = NULL;
static int cross_arch_idx = -1;

/* 
 * correlation between architecture names and elf
 * 'machine' header values
 */
static struct magic_struct MAGIC[] = {
	{ .arch = "arm",            .mach = EM_ARM  },
	{ .arch = "uclibc-arm",     .mach = EM_ARM  },
	{ .arch = "powerpc",        .mach = EM_PPC  },
	{ .arch = "uclibc-powerpc", .mach = EM_PPC  },
	{ .arch = "mips",           .mach = EM_MIPS },
	{ .arch = "uclibc-mips",    .mach = EM_MIPS },
	{ .arch = "mipsel",         .mach = EM_MIPS },
	{ .arch = "uclibc-mipsel",  .mach = EM_MIPS },
	{ .arch = "sh4",            .mach = EM_SH },
	{ .arch = "uclibc-sh4",     .mach = EM_SH },
	{ .arch = "sh4a",           .mach = EM_SH },
	{ .arch = "uclibc-sh4a",    .mach = EM_SH },
	{ .arch = "ppc64",          .mach = EM_PPC64 },
	{ .arch = "i386",           .mach = EM_386 },
	{ .arch = "uclibc-i386",    .mach = EM_386 },
};

#define swapb(x) ( (((x) & 0xff) << 8) | (((x) & 0xff00)) >> 8 )
int is_our_elf(const char *file)
{
	int fd, l;
	Elf32_Ehdr elfh;

	if (cross_arch_idx == -1) return -1;

	fd = open(file, O_RDONLY);
	dprintf("### file=%s\n", file);
	if (fd < 0)
		return -ENOENT;

	l = read(fd, &elfh, sizeof(Elf32_Ehdr));
	close(fd);

	if (l < sizeof(Elf32_Ehdr)) return -1;
	if (elfh.e_machine != MAGIC[cross_arch_idx].mach &&
	    elfh.e_machine != swapb(MAGIC[cross_arch_idx].mach))
		return -1;

	return 0;
}

void cross_init()
{
	int i;

	/* read in cross chroot path */
	fakechroot_cross = getenv("FAKECHROOT_CROSS");
	if (!fakechroot_cross) return;

	/* read in cross architecture; void fakechroot_cross if unset */
	cross_arch = getenv("CROSS_SHELL_ARCH");
	if (!cross_arch) {
		dprintf("### no arch name defined\n");
		goto failure;
	}

	/* find corresponding elf 'machine' header value */
	for (i = 0; i < sizeof(MAGIC)/sizeof(struct magic_struct); i++) {
		dprintf("### -> %s\n", MAGIC[i].arch);
		if (!strcmp(cross_arch, MAGIC[i].arch)) {
			cross_arch_idx = i;
			break;
		}
	}

	if (cross_arch_idx == -1) {
		dprintf("### no magic found for arch %s\n", cross_arch);
		goto failure;
	}

	return;

failure:
	fakechroot_cross = cross_arch = NULL;
}


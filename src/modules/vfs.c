/* Copyright 2014 lynix <lynix47@gmail.com>
 *
 * This file is part of mi3stat.
 *
 * mi3stat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mi3stat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with aerotools-ng. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdint.h>
#include <sys/statvfs.h>

#define TERABYTE (1024ULL * 1024 * 1024 * 1024)
#define GIGABYTE (1024ULL * 1024 * 1024)
#define MEGABYTE (1024ULL * 1024)
#define KILOBYTE (1024ULL)

void put_vfs_free(char *out, void *arg)
{
	struct statvfs vfs_info;
	if (statvfs((char *)arg, &vfs_info) < 0) {
		sprintf(out, "??");
		return;
	}

	uint64_t bytes = (uint64_t)vfs_info.f_bsize*(uint64_t)vfs_info.f_bfree;

	if (bytes > TERABYTE)
			sprintf(out, "%.02f TB", (double)bytes / TERABYTE);
	else if (bytes > GIGABYTE)
			sprintf(out, "%.01f GB", (double)bytes / GIGABYTE);
	else if (bytes > MEGABYTE)
			sprintf(out, "%.01f MB", (double)bytes / MEGABYTE);
	else if (bytes > KILOBYTE)
			sprintf(out, "%.01f KB", (double)bytes / KILOBYTE);
	else {
			sprintf(out, "%.01f B", (double)bytes);
	}
}


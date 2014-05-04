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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BUFLEN 512

/* calculation taken from `vicious` memory widget */

void put_mem_usage(char *out, void *arg)
{
	FILE *statf = fopen("/proc/meminfo", "r");
	if (statf == NULL) {
		sprintf(out, "??%%");
		return;
	}

	uint32_t mem_free=0, mem_buff=0, mem_bufb=0, mem_bufc=0, mem_inuse=0,
			mem_total=0, value=0;
	char name[BUFLEN];

	while (fscanf(statf, "%s %u %*s\n", name, &value) != EOF) {
		if (strcmp("MemTotal:", name) == 0)
			mem_total = value;
		else if (strcmp("MemFree:", name) == 0)
			mem_buff = value;
		else if (strcmp("Buffers:", name) == 0)
			mem_bufb = value;
		else if (strcmp("Cached:", name) == 0)
			mem_bufc = value;
	}
	fclose(statf);

	mem_free  = mem_buff + mem_bufb + mem_bufc;
	mem_inuse = mem_total - mem_free;

	sprintf(out, "%2d%%", (mem_inuse * 100) / mem_total);
}

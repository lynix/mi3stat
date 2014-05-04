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

#include "../global.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TERABYTE (1024ULL * 1024 * 1024 * 1024)
#define GIGABYTE (1024ULL * 1024 * 1024)
#define MEGABYTE (1024ULL * 1024)
#define KILOBYTE (1024ULL)


void put_net_rates(char *out, void *arg)
{
	static uint64_t net_in_prev = 0, net_out_prev = 0;
	uint64_t net_in_cur, net_out_cur;
	double net_in, net_out;
	char *unit_in, *unit_out;

	static char *dev = NULL;
	if (dev == NULL)
		dev = (char *)malloc(11);

	FILE *stat = fopen("/proc/net/dev", "r");
	while (1) {
		if (fscanf(stat, "%10s %lu %*s %*s %*s %*s %*s %*s %*s %lu %*s %*s %*s %*s %*s %*s %*s",
				dev, &net_in_cur, &net_out_cur) == EOF) {
			sprintf(out, "??");
			return;
		}
		// TODO: fix for device names not following ethX scheme
		if (strncmp(dev, (char *)arg, 4) == 0)
			break;
	}
	fclose(stat);

	net_in = (net_in_cur - net_in_prev) / main_interval;
	net_out = (net_out_cur - net_out_prev) / main_interval;

	if (net_in > TERABYTE) {
			net_in /= TERABYTE;
			unit_in = "T/s";
	} else if (net_in > GIGABYTE) {
			net_in /= GIGABYTE;
			unit_in = "G/s";
	} else if (net_in > MEGABYTE) {
		net_in /= MEGABYTE;
		unit_in = "M/s";
	} else if (net_in > KILOBYTE) {
		net_in /= KILOBYTE;
		unit_in = "k/s";
	} else {
		/* lower limit, background noise */
		net_in = 0;
		unit_in = "k/s";
	}

	if (net_out > TERABYTE) {
			net_out /= TERABYTE;
			unit_out = "T/s";
	} else if (net_out > GIGABYTE) {
		net_out /= GIGABYTE;
		unit_out = "G/s";
	} else if (net_out > MEGABYTE) {
		net_out /= MEGABYTE;
		unit_out = "M/s";
	} else if (net_out > KILOBYTE) {
		net_out /= KILOBYTE;
		unit_out = "k/s";
	} else {
		/* lower limit, background noise */
		net_out = 0;
		unit_out = "k/s";
	}

	net_in_prev = net_in_cur;
	net_out_prev = net_out_cur;

	sprintf(out, "%1.2f %s %1.2f %s", net_in, unit_in, net_out, unit_out);
}

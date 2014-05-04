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

void put_cpu_usage(char *out, void *arg)
{
	static int prev_total = 0;
	static int prev_idle  = 0;

	int curr_user = 0, curr_nice = 0, curr_system = 0, curr_idle = 0, curr_total;
	int diff_idle, diff_total, diff_usage;

	FILE *stat = fopen("/proc/stat", "r");
	if (fscanf(stat, "cpu %d %d %d %d", &curr_user, &curr_nice, &curr_system, &curr_idle) != 4) {
		sprintf(out, "??%%");
		fclose(stat);
		return;
	}
	fclose(stat);

	/* adopted from `i3status` */
	curr_total = curr_user + curr_nice + curr_system + curr_idle;
	diff_idle  = curr_idle - prev_idle;
	diff_total = curr_total - prev_total;
	diff_usage = (diff_total ? (1000 * (diff_total - diff_idle)/diff_total + 5)
			/ 10 : 0);
	prev_total = curr_total;
	prev_idle  = curr_idle;

	sprintf(out, "%2d%%", diff_usage);
}

void put_cpu_temp(char *out, void *arg)
{
	static unsigned int prev_temp = 0;
	unsigned int cur_temp, avg_temp;

	FILE *stat = fopen((char *)arg, "r");
	if (fscanf(stat, "%u", &cur_temp) != 1) {
		sprintf(out, "??°C");
		fclose(stat);
		return;
	}
	fclose(stat);

	avg_temp = (cur_temp + prev_temp) / 2000;
	prev_temp = cur_temp;

	sprintf(out, "%d°C", avg_temp);
}

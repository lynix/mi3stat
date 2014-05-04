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

void put_nvidia_temp(char *out, void *arg)
{
	FILE *fp = popen("/usr/bin/nvidia-settings -q gpucoretemp -t", "r");
	if (fp == NULL) {
		sprintf(out, "??°C");
		return;
	}

	int temp;
	if (fscanf(fp, "%d", &temp) != 1) {
		sprintf(out, "??°C");
		fclose(fp);
		return;
	}
	fclose(fp);

	sprintf(out, "%2d°C", temp);
}

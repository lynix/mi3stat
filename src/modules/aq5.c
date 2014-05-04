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
#include <libaquaero5.h>

void put_aq5_temp(char *out, void *arg)
{
	static aq5_data_t aq_data;
	static char *aq_err;

	libaquaero5_poll(NULL, &aq_data, &aq_err);
	sprintf(out, "%.2f°C", aq_data.temp[*(int *)arg]);
}

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
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include <glib.h>

#include "global.h"
#include "modules/cpu.h"
#include "modules/mem.h"
#include "modules/gpu_nvidia.h"
#include "modules/vfs.h"
#include "modules/time.h"
#include "modules/net.h"
#ifdef ENABLE_AQ5
	#include "modules/aq5.h"
#endif

#define CONFIG_DEF  "/.mi3stat.conf"
#define MAININT_DEF 1
#define MAX_LINE    1024

typedef void (*worker_func_t)(char *, void *);

typedef struct {
	short			id;
	short			interval;
	worker_func_t	func;
	void			*arg0;
} worker_args_t;

typedef enum { MODE_JSON, MODE_PLAIN } outp_mode_t;

char 			**buffers;
char			**labels;
uint8_t			num_workers;
pthread_mutex_t	**locks;
char		    *config_filename;
outp_mode_t		output_mode;


void *worker_loop(void *arg)
{
	worker_args_t *args = (worker_args_t *)arg;

	short id = args->id;
	short interval = args->interval;
	worker_func_t worker_func = args->func;
	char *arg0 = args->arg0;

	while (1) {
		pthread_mutex_lock(locks[id]);
			worker_func(buffers[id], arg0);
		pthread_mutex_unlock(locks[id]);
		sleep(interval);
	}

	return NULL;
}

void spawn_worker(worker_func_t func, short interval, char *label, char *arg0)
{
	static uint8_t worker_no = 0;

	labels[worker_no] = label;
	buffers[worker_no] = (char *)malloc(BUFFER_LEN);
	bzero(buffers[worker_no], BUFFER_LEN);
	locks[worker_no] = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(locks[worker_no], NULL);

	pthread_t worker_thread;

	worker_args_t *args = (worker_args_t *)malloc(sizeof(worker_args_t));
	args->id = worker_no;
	args->interval = interval;
	args->arg0 = arg0;
	args->func = func;

	pthread_create(&worker_thread, NULL, worker_loop, (void *)args);

	worker_no++;
}

void print_help()
{
	puts("Usage: mi3stat [OPTIONS]\n");
	puts("OPTIONS:");
	printf("  -c  FILE     use FILE as config file (default: %s)\n",
			CONFIG_DEF);
	puts(  "  -t           plaintext mode (default: JSON output)");
	puts(  "  -h           display this usage information\n");
	printf("This version of mi3stat was built on %s %s.\n", __DATE__, __TIME__);
}

void parse_cmdline(int argc, char *argv[])
{
	char c;
	extern int optind, optopt, opterr;

	while ((c = getopt(argc, argv, "c:i:th")) != -1) {
		switch (c) {
			case 'h':
				print_help();
				exit(EXIT_SUCCESS);
				break;
			case 'c':
				free(config_filename);
				config_filename = optarg;
				break;
			case 't':
				output_mode = MODE_PLAIN;
				break;
			case '?':
				if (optopt == 'd'|| optopt == 'o')
					fprintf(stderr, "option -%c requires an argument\n",
							optopt);
				exit(EXIT_FAILURE);
				break;
			default:
				fprintf(stderr, "invalid arguments. Try -h for help.");
				exit(EXIT_FAILURE);
		}
	}
}

void parse_config()
{
	GKeyFile *config = g_key_file_new();
	GError *error = NULL;
	if (g_key_file_load_from_file(config, config_filename,	G_KEY_FILE_NONE,
			&error) != 1) {
		fprintf(stderr, "failed to parse config file '%s': %s\n",
				config_filename, error->message);
		exit(EXIT_FAILURE);
	}

	gsize length;
	gchar **modules = g_key_file_get_groups(config, &length);
	num_workers = length;

	// global settings module
	if (g_key_file_has_group(config, "global")) {
		num_workers--;
		// main output interval
		if (g_key_file_has_key(config, "global", "interval", &error))
			main_interval = (uint8_t)g_key_file_get_integer(config, "global",
					"interval", &error);
	}

	buffers = (char **)malloc(num_workers * sizeof(char *));
	labels = (char **)malloc(num_workers * sizeof(char *));
	locks = (pthread_mutex_t **)malloc(num_workers * sizeof(pthread_mutex_t *));
	if (buffers == NULL || labels == NULL || locks == NULL) {
		fprintf(stderr, "failed to allocate data structures\n");
		exit(EXIT_FAILURE);
	}

	for (gsize i=0; i<length; i++) {
		char *module = modules[i];

		// skip global module
		if (strcmp(module, "global") == 0)
			continue;

		// regular modules
		if (!g_key_file_has_key(config, module, "type", &error)) {
			fprintf(stderr, "config error: module '%s' missing 'type' field.\n",
					module);
			exit(EXIT_FAILURE);
		}
		char *type = g_key_file_get_string(config, module, "type", &error);

		char *prefix = "";
		if (g_key_file_has_key(config, module, "prefix", &error))
			prefix = g_key_file_get_string(config, module, "prefix", &error);

		uint32_t interval = main_interval;
		if (g_key_file_has_key(config, module, "interval", &error))
			interval = g_key_file_get_integer(config, module, "interval",
					&error);

#ifdef ENABLE_AQ5
		if (strcmp(type, MOD_AQ5_TEMP) == 0) {
			spawn_worker(&put_aq5_temp, interval, prefix, NULL);
			continue;
		}
#endif
		if (strcmp(type, MOD_CPU_TEMP) == 0) {
			char *tempfile = g_key_file_get_string(config, module, "sensor",
					&error);
			if (access(tempfile, R_OK) != 0)
				fprintf(stderr, "unable to read cpu temperature file '%s'\n",
						tempfile);
			else
				spawn_worker(&put_cpu_temp, interval, prefix, tempfile);
			continue;
		}
		if (strcmp(type, MOD_CPU_USAGE) == 0) {
			spawn_worker(&put_cpu_usage, interval, prefix, NULL);
			continue;
		}
		if (strcmp(type, MOD_MEM_USAGE) == 0) {
			spawn_worker(&put_mem_usage, interval, prefix, NULL);
			continue;
		}
		if (strcmp(type, MOD_NVIDIA_TEMP) == 0) {
			spawn_worker(&put_nvidia_temp, interval, prefix, NULL);
			continue;
		}
		if (strcmp(type, MOD_VFS_FREE) == 0) {
			char *path = g_key_file_get_string(config, module, "path", &error);
			if (access(path, F_OK) != 0)
				fprintf(stderr, "config error: invalid path: '%s'\n", path);
			else
				spawn_worker(&put_vfs_free, interval, prefix, path);
			continue;
		}
		if (strcmp(type, MOD_NET_RATES) == 0) {
			char *device = g_key_file_get_string(config, module, "device",
					&error);
			spawn_worker(&put_net_rates, interval, prefix, device);
			continue;
		}
		if (strcmp(type, MOD_DATETIME) == 0) {
			char *format = g_key_file_get_string(config, module, "format",
					&error);
			spawn_worker(&put_datetime, interval, prefix, format);
			continue;
		}
	}
}

int main(int argc, char *argv[])
{
	// initialization
	char *home = getenv("HOME");
	config_filename = malloc(strlen(home) + strlen(CONFIG_DEF) +1);
	config_filename = strcpy(config_filename, home);
	config_filename = strcat(config_filename, CONFIG_DEF);

	// initialize default settings
	output_mode = MODE_JSON;
	main_interval = MAININT_DEF;

	// parse commandline
	parse_cmdline(argc, argv);

	// parse configuration file, spawn workers
	parse_config();


	char status_line[MAX_LINE];
	char *p = status_line;

	close(STDIN_FILENO);

	if (output_mode == MODE_JSON)
		puts("{\"version\":1}\n[\n");

	// TODO: beautify, extract color and separator width
	char *cellfmt_str;
	if (output_mode == MODE_JSON) {
		cellfmt_str = "{ \"full_text\": \"%s%s\", \"color\": \"#ffffff\", \"separator_block_width\": 19 },\n";
	} else {
		cellfmt_str = "%s%s | ";
	}

	// main loop
	while (1) {
		p = status_line;
		if (output_mode == MODE_JSON)
			*(p++) = '[';

		for (uint8_t i=0; i<num_workers; i++) {
			pthread_mutex_lock(locks[i]);
				p += sprintf(p, cellfmt_str, labels[i], buffers[i]);
			pthread_mutex_unlock(locks[i]);
		}

		if (output_mode == MODE_JSON) {
			*(p-2) = ']';
			*(p-1) = ',';
			*(p++) = '\0';
		} else {
			// cut off trailing separator
			*(p-3) = '\0';
		}

		puts(status_line);
		fflush(stdout);

		sleep(main_interval);
	}

	return 0;
}

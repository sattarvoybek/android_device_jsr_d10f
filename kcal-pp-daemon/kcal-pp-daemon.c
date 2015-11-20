/*
 * Copyright (C) 2015 S-trace
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cutils/properties.h>
#include <cutils/sockets.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define _GNU_SOURCE
#include <stdio.h>

#define LOG_TAG "kcal-pp-daemon"
/* #define LOG_NDEBUG 0 */
#include <log/log.h>

#include "kcal-pp-daemon.h" // Look to this header for configuration

int enabled = FALSE;
int hue = 0;
double saturation = 0.0;
int intensity = 0;
double contrast = 0.0;

void load_props(void) {
	char prop_value[PROPERTY_VALUE_MAX];

	INFO("Loading props\n");

	enabled = property_get_int32(PROP_PP_ENABLED, 0);
	hue = property_get_int32(PROP_PP_HUE, 0);
	intensity = property_get_int32(PROP_PP_INTENSITY, 0);

	property_get(PROP_PP_SATURATION, prop_value, "0.0");
	sscanf(prop_value, "%lf", &saturation);

	property_get(PROP_PP_CONTRAST, prop_value, "0.0");
	sscanf(prop_value, "%lf", &contrast);
}

void save_props(int hue, double saturation, int intensity, double contrast) {
	char prop_value[PROPERTY_VALUE_MAX];

	INFO("Saving props\n");

	sprintf(prop_value, "%d", hue);
	property_set(PROP_PP_HUE, prop_value);

	sprintf(prop_value, "%f", saturation);
	property_set(PROP_PP_SATURATION, prop_value);

	sprintf(prop_value, "%d", intensity);
	property_set(PROP_PP_INTENSITY, prop_value);

	sprintf(prop_value, "%f", contrast);
	property_set(PROP_PP_CONTRAST, prop_value);
}

int write_kcal_file(char *name, int value) {
	errno = 0;
	int buf_size = strlen(KCAL_PATH)+strlen(name)+1;
	char *full_name = calloc(sizeof(char), buf_size );
	if (NULL == full_name) {
		ERROR("calloc() failed (%s)\n", strerror(errno));
		return FALSE;
	}
	errno = 0;
	snprintf(full_name, buf_size, "%s%s", KCAL_PATH, name);
	if (errno) {
		ERROR("snprintf() failed (%s)\n", strerror(errno));
		free(full_name);
		return FALSE;
	}
	FILE *file = fopen(full_name, "a+");
	if (NULL == file) {
		ERROR("Failed to open '%s' (%s)\n", full_name, strerror(errno));
		free(full_name);
		return FALSE;
	}
	free(full_name);

	errno = 0;
	if (fprintf(file, "%d", value) < 0) {
		ERROR("Failed to write '%s%s' (%s)\n", KCAL_PATH, name, strerror(errno));
		fclose(file);
		return FALSE;
	}
	fclose(file);
	return TRUE;
}

int kcal_write_values(int hue, int saturation, int intensity, int contrast) {
	INFO("Writing values to kcal\n");
	int errors = 0;
	errors += write_kcal_file(KCAL_HUE_FILE, hue)        ? 0 : 1;
	errors += write_kcal_file(KCAL_SAT_FILE, saturation) ? 0 : 1;
	errors += write_kcal_file(KCAL_INT_FILE, intensity)  ? 0 : 1;
	errors += write_kcal_file(KCAL_CNT_FILE, contrast)   ? 0 : 1;
	if (errors > 0) {
		ERROR("Failed to write %d values!\n", errors);
	}
	else {
		INFO("Values written\n");
	}
	return errors > 0 ? FALSE : TRUE;
}

int set_values(int socket_fd, char *buf) {
	int result = 0;

	int kcal_hue = KCAL_DEF_HUE;
	int kcal_saturation = KCAL_DEF_SAT;
	int kcal_intensity = KCAL_DEF_INT;
	int kcal_contrast = KCAL_DEF_CNT;

	if (4 == sscanf(buf, REQUEST_FORMAT, &hue, &saturation,
	                &intensity, &contrast)) {
		DEBUG("Parsed: " REQUEST_FORMAT, hue, saturation, intensity, contrast);
		save_props(hue, saturation, intensity, contrast);

// Do some math to convert values recieved from PPPreference.apk to kcal
		if (hue < 0) {
			kcal_hue = KCAL_MAX_HUE-((KCAL_MAX_HUE/2)*((-1*hue)/180.0));
		}
		else {
			kcal_hue = (KCAL_MAX_HUE/2)*(hue/180.0);
		}
		if (saturation < 0) {
			kcal_saturation = KCAL_DEF_SAT+((KCAL_DEF_SAT-KCAL_MIN_SAT)*saturation);
		}
		else {
			kcal_saturation = KCAL_DEF_SAT+((KCAL_MAX_SAT-KCAL_DEF_SAT)*saturation);
		}
		if (intensity < 0) {
			kcal_intensity = KCAL_DEF_INT+((KCAL_DEF_INT-KCAL_MIN_INT)*intensity/255);
		}
		else {
			kcal_intensity = KCAL_DEF_INT+((KCAL_MAX_INT-KCAL_DEF_INT)*intensity/255);
		}
		if (contrast < 0) {
			kcal_contrast = KCAL_DEF_CNT+((KCAL_DEF_CNT-KCAL_MIN_CNT)*contrast);
		}
		else {
			kcal_contrast = KCAL_DEF_CNT+((KCAL_MAX_CNT-KCAL_DEF_CNT)*contrast);
		}
		DEBUG("Recalc: h = %d s = %d i = %d c = %d", kcal_hue, kcal_saturation, kcal_intensity, kcal_contrast);
		result = kcal_write_values(kcal_hue, kcal_saturation, kcal_intensity, kcal_contrast);
	}
	else {
		ERROR("Unable to parse set request '%s' parameters!\n", buf);
		result = FALSE;
	}
	if (result) {
		if (-1 == write(socket_fd, REPLY_SUCCESS, strlen(REPLY_SUCCESS))) {
			ERROR("Failed to write to socket %d (%s), terminating thread", socket_fd, strerror(errno));
			return (-1);
		}
	}
	else {
		if (-1 == write(socket_fd, REPLY_FAILURE, strlen(REPLY_FAILURE))) {
			ERROR("Failed to write to socket %d (%s), terminating thread", socket_fd, strerror(errno));
			return (-1);
		}
	}
	return (result);
}

int get_kcal_state(void) {
	errno = 0;
	int buf_size = strlen(KCAL_PATH)+strlen(KCAL_ENABLE_FILE)+1;
	char *full_name = calloc(sizeof(char), buf_size );
	if (NULL == full_name) {
		ERROR("calloc() failed (%s)\n", strerror(errno));
		return 1;
	}
	errno = 0;
	snprintf(full_name, buf_size, "%s%s", KCAL_PATH, KCAL_ENABLE_FILE);
	if (errno) {
		ERROR("sprintf() failed (%s)\n", strerror(errno));
		free(full_name);
		return 1;
	}
	FILE *file = fopen(full_name, "r");
	if (NULL == file) {
		ERROR("Failed to open '%s' (%s)\n", full_name, strerror(errno));
		free(full_name);
		return 1;
	}
	free(full_name);

	errno = 0;
	int value = 0;
	if (1 != fscanf(file, "%d", &value)) {
		ERROR("fscanf() failed: '%s%s' (%s)\n", KCAL_PATH, KCAL_ENABLE_FILE, strerror(errno));
		return -1;
	}
	if (fclose(file) != 0) {
		ERROR("Failed to close '%s%s' (%s)\n", KCAL_PATH, KCAL_ENABLE_FILE,
		      strerror(errno));
	}
	if (0 == value)
		return FALSE;
	else
		return TRUE;
	return -1;
}

int pp_enable(int socket_fd, int state) {
	if (get_kcal_state() == state) {
		DEBUG("%s: KCAL state already match requested", __FUNCTION__);
		char prop_value[PROPERTY_VALUE_MAX];
		sprintf(prop_value, "%d", state);
		property_set(PROP_PP_ENABLED, prop_value);
		return TRUE;
	}
	if (write_kcal_file(KCAL_ENABLE_FILE, state)) {
		if (-1 == write(socket_fd, REPLY_SUCCESS, strlen(REPLY_SUCCESS))) {
			ERROR("Failed to write to socket %d (%s), terminating thread",
			      socket_fd, strerror(errno));
			return (FALSE);
		}
	}
	else {
		char prop_value[PROPERTY_VALUE_MAX];
		sprintf(prop_value, "%d", state);
		property_set(PROP_PP_ENABLED, prop_value);
		if (-1 == write(socket_fd, REPLY_FAILURE, strlen(REPLY_FAILURE))) {
			ERROR("Failed to write to socket %d (%s), terminating thread",
			      socket_fd, strerror(errno));
			return (FALSE);
		}
	}
	return (TRUE);
}

void *main_loop(void *arg) {
	int socket_fd = *(int *)arg;
	char buf[128]; // Socket read buffer
	INFO("socket_fd = %d\n", socket_fd);
	while (1) {
		memset(buf, 0, sizeof(buf));
		int read_bytes = read(socket_fd, buf, sizeof(buf));
		if ( 0 == read_bytes ) {
			INFO("Socket %d closed, terminating thread\n", socket_fd);
			return NULL;
		}
		if ( -1 == read_bytes ) {
			INFO("Failure reading from %d (%s), terminating thread\n",
			     socket_fd, strerror(errno));
			return NULL;
		}
		INFO("Recieved %d bytes: '%s'\n", read_bytes, buf);
		if (!strncmp(buf, REQUEST_PP_ON, MIN((signed)strlen(REQUEST_PP_ON), read_bytes))) {
			INFO("PP enable request recieved\n");
			if (!pp_enable(socket_fd, 1)) {
				return NULL;
			}
			continue;
		}
		if (!strncmp(buf, REQUEST_PP_OFF, MIN((signed)strlen(REQUEST_PP_OFF), read_bytes))) {
			INFO("PP disable request recieved\n");
			set_values(socket_fd, REQUEST_RESET);
			if (!pp_enable(socket_fd, 0)) {
				return NULL;
			}
			continue;
		}
		if (!strncmp(buf, REQUEST_STATUS, MIN((signed)strlen(REQUEST_STATUS), read_bytes))) {
			INFO("Status request recieved\n");
			if (get_kcal_state()) {
				if (-1 == write(socket_fd, REPLY_RUNNING, strlen(REPLY_RUNNING))) {
					ERROR("Failed to write to socket %d (%s), terminating thread",
					      socket_fd, strerror(errno));
					return NULL;
				}
			}
			else {
				if (-1 == write(socket_fd, REPLY_RUNNING, strlen(REPLY_STOPPED))) {
					ERROR("Failed to write to socket %d (%s), terminating thread",
					      socket_fd, strerror(errno));
					return NULL;
				}
			}
			continue;
		}
		if (!strncmp(buf, REQUEST_SET, MIN((signed)strlen(REQUEST_SET), read_bytes))) {
			INFO("Set command recieved\n");
			if (!set_values(socket_fd, buf)) {
				ERROR("Failed to set values - terminating thread");
				return NULL;
			}
			continue;
		}
		ERROR("Something werid recieved: %s\n", buf);
	}
}

void usage(void) {
	printf("kcal UI postprocessing daemon v0.00\n");
	printf("By S-trace (S-trace@list.ru)\n");
	printf("Please start this programm via init.rc like this:\n\n");
	printf("service ppd /system/bin/kcal-pp-daemon\n");
	printf(" class late_start\n");
	printf(" user system\n");
	printf(" socket pps stream 0660 system system graphics\n");
	printf(" group system graphics\n\n");
	printf("on property:init.svc.surfaceflinger=stopped\n");
	printf(" stop ppd\n\n");
}

int main (int argc, char **argv) {
	if (argc > 1) {
		usage();
		exit(EXIT_SUCCESS);
	}
	INFO("Daemon starting\n");
	int control_socket = android_get_control_socket(CONTROL_SOCKET_NAME);
	if (control_socket < 0) {
		ERROR("Failed to get control socket(%s): %s\n\
Is daemon started by init?\n\
Try %s --usage\n", CONTROL_SOCKET_NAME, strerror(errno), argv[0]);
		usage();
		exit(EXIT_FAILURE);
	}
	load_props();
	char request[PROPERTY_VALUE_MAX*5];
	sprintf(request, REQUEST_FORMAT, hue, saturation, intensity, contrast);
	INFO("Writing saved values to kcal: %s\n", request);
	set_values(1, request);

	INFO("Listening for connections\n");
	if (listen(control_socket, 4) < 0) {
		ERROR("Failed to listen control socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct sockaddr addr;
	socklen_t alen = sizeof(addr);
	int thread_count = 0;
	while (TRUE) {
		int socket_fd = accept(control_socket, &addr, &alen);
		if (socket_fd < 0) {
			ERROR("Failed to accept control socket: %s\n", strerror(errno));
			continue;
		}
		pthread_t thread_id;
		pthread_create(&thread_id, NULL, &main_loop, &socket_fd);
		INFO("Spawned brand new thread");
	}
	ERROR("Daemon finished\n");
	return EXIT_FAILURE;
}

#include <stdlib.h>
#include <stdio.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "init.h"
#include "util.h"
#include "init.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/statvfs.h>

#include "init_msm.h"

#define PERSISTENT_PROPERTY_DIR  "/data/property"
#define PERSISTENT_PROPERTY_CONFIGURATION_NAME "persist.storages.configuration"
#define STORAGES_CONFIGURATION_CLASSIC   "0"
#define STORAGES_CONFIGURATION_INVERTED  "1"
#define STORAGES_CONFIGURATION_DATAMEDIA "2"
#define SERVICE_VOLD "vold"
void init_msm_properties(unsigned long msm_id, unsigned long msm_ver, char * board_type)
{
	UNUSED(msm_id);
	UNUSED(msm_ver);
	UNUSED(board_type);

	char value[PROP_VALUE_MAX];
	int rc;

	DIR * dir = opendir(PERSISTENT_PROPERTY_DIR);
	int dir_fd;
	struct dirent * entry;
	int fd, length;
	struct stat sb;

	if (dir) {
		dir_fd = dirfd(dir);
		while ((entry = readdir(dir)) != NULL) {
			// we need to read this properties before load_persistent_properties()
			if (strncmp(PERSISTENT_PROPERTY_CONFIGURATION_NAME, entry->d_name, strlen(PERSISTENT_PROPERTY_CONFIGURATION_NAME)))
				continue;
#if HAVE_DIRENT_D_TYPE
			if (entry->d_type != DT_REG)
				continue;
#endif
			/* open the file and read the property value */
			fd = openat(dir_fd, entry->d_name, O_RDONLY | O_NOFOLLOW);
			if (fd < 0) {
				ERROR("Unable to open persistent property file \"%s\" errno: %d\n", entry->d_name, errno);
				continue;
			}
			if (fstat(fd, &sb) < 0) {
				ERROR("fstat on property file \"%s\" failed errno: %d\n", entry->d_name, errno);
				close(fd);
				continue;
			}

			// File must not be accessible to others, be owned by root/root, and
			// not be a hard link to any other file.
			if (((sb.st_mode & (S_IRWXG | S_IRWXO)) != 0)
				      || (sb.st_uid != 0)
				      || (sb.st_gid != 0)
				      || (sb.st_nlink != 1)) {
				ERROR("skipping insecure property file %s (uid=%u gid=%u nlink=%d mode=%o)\n",
				      entry->d_name, (unsigned int)sb.st_uid, (unsigned int)sb.st_gid,
				      sb.st_nlink, sb.st_mode);
				close(fd);
				continue;
			}

			length = read(fd, value, sizeof(value) - 1);
			if (length >= 0) {
				value[length] = 0;
				property_set(entry->d_name, value);
			} else {
				ERROR("Unable to read persistent property file %s errno: %d\n", entry->d_name, errno);
			}
			close(fd);
		}
		closedir(dir);
	} else {
		ERROR("Unable to open persistent property directory %s errno: %d\n", PERSISTENT_PROPERTY_DIR, errno);
	}

        unsigned long mount_flags=0;
	struct statvfs statvfs_buf;
	if (statvfs("/init", &statvfs_buf) != 0) {
		ERROR("statvfs() failed, errno: %d (%s)\n", errno, strerror(errno));
	}
	else {
		mount_flags=statvfs_buf.f_flag;
	}

	mount("rootfs", "/", "rootfs", MS_REMOUNT|0, NULL);

	rc = property_get(PERSISTENT_PROPERTY_CONFIGURATION_NAME, value);
	if (rc && ISMATCH(value, STORAGES_CONFIGURATION_DATAMEDIA)) {
		// if datamedia
		ERROR("Got datamedia storage configuration (" PERSISTENT_PROPERTY_CONFIGURATION_NAME " == %s)\n", value);
		unlink("/fstab.d10f");
		link("/fstab.d10f_int", "/fstab.d10f");
		unlink("/fstab.d10f_sd");
		unlink("/fstab.d10f_int");
	} else if (rc && ISMATCH(value, STORAGES_CONFIGURATION_INVERTED)) {
		// if swapped
		property_set("ro.vold.primary_physical", "1");
		ERROR("Got inverted storage configuration (" PERSISTENT_PROPERTY_CONFIGURATION_NAME " == %s)\n", value);
		unlink("/fstab.d10f");
		link("/fstab.d10f_sd", "/fstab.d10f");
		unlink("/fstab.d10f_sd");
		unlink("/fstab.d10f_int");
	} else {
		// if classic (default case)
		property_set("ro.vold.primary_physical", "1");
		ERROR("Got classic storage configuration (" PERSISTENT_PROPERTY_CONFIGURATION_NAME " == %s)\n", value);
		unlink("/fstab.d10f_sd");
		unlink("/fstab.d10f_int");
	}
	ERROR("Storage configuration applied\n");

	struct service *svc = service_find_by_name(SERVICE_VOLD);
	if (svc) {
		ERROR("Restarting vold\n");
		service_restart(svc);
		ERROR("Restarted vold)\n");
	} else {
		ERROR("no such service '%s'\n", SERVICE_VOLD);
	}

	mount("rootfs", "/", "rootfs", MS_REMOUNT|mount_flags, NULL);
}

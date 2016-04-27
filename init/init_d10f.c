#include <stdlib.h>
#include <stdio.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "util.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mount.h>


#include "init_msm.h"

#define SWAP_PCH(_a_,_b_) { const char * x = (_a_); (_a_) = (_b_); (_b_) = x; }
#define SWAP_INT(_a_,_b_) { int x = (_a_); (_a_) = (_b_); (_b_) = x; }

#define PERSISTENT_PROPERTY_DIR  "/data/property"
#define PERSISTENT_PROPERTY_PLANNED_SWAP    "persist.storages.planned_swap"
#define PERSISTENT_PROPERTY_SWAPPED         "persist.storages.swapped"

static const char * prop_skip_list[] = {
	PERSISTENT_PROPERTY_PLANNED_SWAP,
};


#define FILE_STORAGE_LIST  "/data/system/storage_list.xml"

typedef struct storage_item {
	char * mountPoint;
	char * storageDescription;
	int primary;
	int removable;
	int emulated;
	int mtpReserve;
	int allowMassStorage;
	int maxFileSize;
};

static struct storage_item storage_list[] = {
	{
		.mountPoint = "/storage/sdcard0",
		.storageDescription = "@string/storage_internal",
		.primary = 1,
		.removable = 0,
		.allowMassStorage = 0,
	},
	{
		.mountPoint = "/storage/sdcard1",
		.storageDescription = "@string/storage_sd_card",
		.primary = 0,
		.removable = 1,
		.allowMassStorage = 1,
	},
	{
		.mountPoint = "/storage/usbdisk",
		.storageDescription = "@string/storage_usb",
		.primary = 0,
		.removable = 1,
		.allowMassStorage = 0,
	},
};


int check_skip_list(const char * prop_name)
{
	size_t i;
	size_t len = strlen(prop_name);
	for (i = 0; i < ARRAY_SIZE(prop_skip_list); i++) {
		if (prop_skip_list[i] && strlen(prop_skip_list[i]) == len) {
			if (strcmp(prop_skip_list[i], prop_name) == 0) {
				return 1; 
			}
		}
	}
	return 0;
}

int init_prop_list(void)
{
	char value[PROP_VALUE_MAX];
	DIR * dir = opendir(PERSISTENT_PROPERTY_DIR);
	int dir_fd;
	struct dirent * entry;
	int fd, length;
	struct stat sb;

	if (dir) {
		dir_fd = dirfd(dir);
		while ((entry = readdir(dir)) != NULL) {
			// we need to read this properties before load_persistent_properties()
			if (check_skip_list(entry->d_name))
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
	return 0;
}

int create_storage_list(void)
{
	size_t i;
	FILE * f = fopen(FILE_STORAGE_LIST, "w+");
	if (f == NULL) {
		ERROR("can not open '%s', err: %s\n", FILE_STORAGE_LIST, strerror(errno));
		return -1;
	}
	fprintf(f, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	fprintf(f, "<StorageList>\n");
	for (i = 0; i < ARRAY_SIZE(storage_list); i++) {
		struct storage_item * s = &storage_list[i];
		fprintf(f, "  <storage mountPoint=\"%s\" \n", s->mountPoint);
		fprintf(f, "    storageDescription=\"%s\" \n", s->storageDescription);
		fprintf(f, "    primary=\"%s\" \n", s->primary ? "true" : "false");
		fprintf(f, "    removable=\"%s\" \n", s->removable ? "true" : "false");
		fprintf(f, "    emulated=\"%s\" \n", s->emulated ? "true" : "false");
		fprintf(f, "    allowMassStorage=\"%s\" \n", s->allowMassStorage ? "true" : "false");
		fprintf(f, "  /> \n");
	}
	fprintf(f, "</StorageList>\n");
	fclose(f);
	return 0;
}

void init_msm_properties(unsigned long msm_id, unsigned long msm_ver, char * board_type)
{
	UNUSED(msm_id);
	UNUSED(msm_ver);
	UNUSED(board_type);

	int rc;
	char value[PROP_VALUE_MAX];

	init_prop_list();

	mount("rootfs", "/", "rootfs", MS_REMOUNT|0, NULL);

	rc = property_get(PERSISTENT_PROPERTY_PLANNED_SWAP, value);
	if (rc && atoi(value)) {
		SWAP_PCH(storage_list[0].mountPoint, storage_list[1].mountPoint);
		SWAP_INT(storage_list[0].primary, storage_list[1].primary);
		property_set(PERSISTENT_PROPERTY_SWAPPED, "1");
		symlink("/fstab.sd", "/fstab.qcom");
	} else {
		property_set(PERSISTENT_PROPERTY_SWAPPED, "0");
		symlink("/fstab.int", "/fstab.qcom");
	}

	mount("rootfs", "/", "rootfs", MS_REMOUNT|MS_RDONLY, NULL);

	create_storage_list();
}

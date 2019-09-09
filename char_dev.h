#ifndef CHAR_DEV_H_
#define CHAR_DEV_H_
#include <linux/cdev.h>

struct char_dev {
	char *message;
	ssize_t size = 4096;
	struct cdev char_device;
};

#endif

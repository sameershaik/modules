#ifndef CHAR_DEV_H_
#define CHAR_DEV_H_
#include <linux/cdev.h>

struct my_device {
	char *message;
	unsigned long size;
	struct cdev char_device;
};

#endif

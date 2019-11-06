#ifndef CHAR_DEV_H_
#define CHAR_DEV_H_
#include <linux/cdev.h>

struct my_device {
	char *message;
	unsigned long size;
	loff_t retain_off;
	struct cdev char_device;
};

#endif

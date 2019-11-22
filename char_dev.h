#ifndef CHAR_DEV_H_
#define CHAR_DEV_H_
#include <linux/cdev.h>
#include <linux/semaphore.h>

struct my_device {
	char *message;
	unsigned long size;
	loff_t retain_off;
	struct semaphore char_sem;
	struct cdev char_device;
};

#undef PDEBUG
#ifdef SCULL_DEBUG
#ifdef __KERNEL__
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"scull:"fmt, ##args)
#else
#define PDEBUG(fmt,args...) fprintf(stderr,fmt, ##args)
#endif
#else
#define PDEBUG(fmt, args...)
#endif


#endif

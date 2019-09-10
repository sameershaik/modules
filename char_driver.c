#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>


#define DEVICE "sameer_char"

static int major;
static int minor;
static dev_t device_number;
static struct class *cl = NULL;

static int start_here(void)
{
	int ret;

	ret = alloc_chrdev_region(&device_number, 0, 1, DEVICE);

	if (ret != 0) {
		printk(KERN_WARNING"gettting the device major number failed\n");
		return -1;
	}

	major = MAJOR(device_number);
	minor = MINOR(device_number);

	cl = class_create(THIS_MODULE, "my_char");

	if (cl == NULL)
		goto fail;

	if (device_create(cl, NULL, device_number, NULL, DEVICE) == NULL)
		goto fail1;

	return 0;

fail1:
	class_destroy(cl);

fail:
	unregister_chrdev_region(device_number, 1);
	return -1;
}


static void end_here(void)
{
	printk(KERN_INFO"Going out of this bloody world\n");
	device_destroy(cl, device_number);
	class_destroy(cl);
	unregister_chrdev_region(device_number, 1);
}

MODULE_LICENSE("GPL");
module_init(start_here);
module_exit(end_here);

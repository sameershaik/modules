#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>


#define DEVICE "sameer_char"

static int major;
static int minor;
static dev_t device_number;
static struct class *cl;

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
		unregister_chrdev_region(device_number, 1);
	
	if (device_create(cl, NULL, device_number, NULL, DEVICE) == NULL) {
		class_destroy(cl);
		unregister_chrdev_region(device_number, 1);
	}

		
	return 0;
}


static void end_here(void)
{
	device_destroy(cl, device_number);
	class_destroy(cl);
	unregister_chrdev_region(device_number, 1);
	printk(KERN_INFO"good bye world\n");
}

MODULE_LICENSE("GPL");
module_init(start_here);
module_exit(end_here);

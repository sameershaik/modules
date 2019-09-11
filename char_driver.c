#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include "char_dev.h"
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE "sameer_char"
#define CLS "my_char"
#define BUFFER 4096

static dev_t device_number;
static struct class *cl = NULL;

static int sameer_open(struct inode *inode, struct file *filp)
{
	struct my_device *dev;
	dev = container_of(inode->i_cdev, struct my_device, char_device);

	// filp->private_data = inode->i_cdev;

	dev->size = BUFFER;
	dev->message = kmalloc(dev->size, GFP_KERNEL);

	if (dev->message == NULL)
		return -ENOMEM;

	return 0;
}

static ssize_t sameer_read(struct file *filp, char __user *buf, size_t count,
			   loff_t *pos)
{
	struct my_device *dev = filp->private_data;
	int ret = -E2BIG;

	// if (filp->f_pos + count > dev->size)
	if (*pos + count > dev->size)
		count = dev->size;

	if (copy_to_user(buf, dev->message, count)) {
		printk(KERN_WARNING"copying to the userspace failed\n");
		ret = -EFAULT;
		goto out;
	}

	// filp->f_pos = filp->f_pos + count;
	pos += count;
	ret = count;

out:
	return ret;
}

static ssize_t sameer_write(struct file *filp, const char __user *buf, size_t
			    count, loff_t *pos)
{
	struct my_device *dev = filp->private_data;
	int ret = -E2BIG;

	// if (filp->f_pos + count > dev->size) {
	if (*pos + count > dev->size) {
		printk(KERN_WARNING"writing to this device will cause the\
		       overflow\n");
		goto out;
	}

	if(copy_from_user(dev->message, buf, count)) {
		printk(KERN_WARNING"Copying from the user space failed\n");
		ret = -EFAULT;
		goto out;
	}

	// filp->f_pos = filp->f_pos + count;
	pos +=  count;
	ret = count;

out:
	return ret;
}

static int sameer_close(struct inode *inode, struct file *filp)
{
	struct my_device *dev = filp->private_data;
	kfree(dev->message);
	return 0;
}

static struct file_operations sops = {
	.open = sameer_open,
	.read = sameer_read,
	.write = sameer_write,
	.release = sameer_close,
	.owner = THIS_MODULE
};

static int start_here(void)
{
	int ret;
	struct device *dev = NULL;
	struct my_device my_device_1 = {0};
	
	ret = alloc_chrdev_region(&device_number, 0, 1, DEVICE);
	if (ret != 0) {
		printk(KERN_WARNING"gettting the device major number failed\n");
		return ret;
	}

	cl = class_create(THIS_MODULE, CLS);

	if (IS_ERR(cl))
		goto fail;

	dev = device_create(cl, NULL, device_number, NULL, DEVICE);
	if (IS_ERR(dev))
		goto fail1;

	cdev_init(&my_device_1.char_device, &sops);

	ret = cdev_add(&my_device_1.char_device, device_number, 1);

	if (ret) {
		printk(KERN_ERR"unabe to add the character device\n");
		goto fail1;
	}
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

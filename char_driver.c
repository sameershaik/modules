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
static struct my_device *my_device_1;

static int sameer_open(struct inode *inode, struct file *filp)
{
	struct my_device *dev;
	dev = container_of(inode->i_cdev, struct my_device, char_device);

	filp->private_data = dev;

	return 0;
}
static loff_t sameer_seek(struct file *filp, loff_t offset, int whence)
{
	struct my_device *dev = filp->private_data;
	loff_t newpos;
	
	switch(whence) {
	case 0:
		printk(KERN_INFO "case %d", whence);
		newpos = offset;
		break;
	case 1:
		printk(KERN_INFO "case %d", whence);
		newpos = filp->f_pos + offset;
		break;
	case 2:
		printk(KERN_INFO "case %d", whence);
		newpos = dev->size + offset;
		break;
	default:
		printk(KERN_INFO"whence is unknown\n");
	}
	if (newpos < 0)
		return -EINVAL;
	
	filp->f_pos = newpos;
	
	return newpos;
}
static ssize_t sameer_read(struct file *filp, char __user *buf, size_t count,
			   loff_t *pos)
{
	int ret;
	struct my_device *dev = (struct my_device *)filp->private_data;

	if (filp->f_pos + count > dev->size) {
		printk(KERN_INFO "its passing this case\n");
		count = dev->size - filp->f_pos;
	}
	if (count > dev->size)
		count = dev->size;
	
	printk("The file_pos is %lld\t count is %d\n", filp->f_pos, count);
	if (copy_to_user(buf, (dev->message + filp->f_pos), count)) {
		printk(KERN_WARNING"copying to the userspace failed\n");
		ret = -EFAULT;
		goto out;
	}

	*pos += count;
	ret = count;
	printk("The pos is %lld\t count is %d\n", *pos, count);
out:
	return ret;
}

static ssize_t sameer_write(struct file *filp, const char __user *buf, size_t
			    count, loff_t *pos)
{
	struct my_device *dev = (struct my_device *)filp->private_data;
	int ret;
	
	if (*pos + count > dev->size) {
		printk(KERN_WARNING"writing to this device will cause the\
		       overflow\n");
		goto out;
	}
		
	printk(KERN_INFO"The offset is %lld, count = %d\n", *pos, count);

	if(copy_from_user((dev->message + *pos), buf, count)) {
		printk(KERN_WARNING"Copying from the user space failed\n");
		ret = -EFAULT;
		goto out;
	}


	*pos +=  count;
	ret = count;

out:
	return ret;
}

static int sameer_close(struct inode *inode, struct file *filp)
{
	/* struct my_device *dev = filp->private_data;
	kfree(dev->message); */
	return 0;
}

static struct file_operations sops = {
	.open = sameer_open,
	.read = sameer_read,
	.write = sameer_write,
	.llseek = sameer_seek,
	.release = sameer_close,
	.owner = THIS_MODULE
};

static int start_here(void)
{
	int ret;
	struct device *dev = NULL;
		
	ret = alloc_chrdev_region(&device_number, 0, 1, DEVICE);
	if (ret != 0) {
		printk(KERN_WARNING"gettting the device major number failed\n");
		return ret;
	}

	my_device_1 = (struct my_device *)kzalloc(sizeof(struct my_device), GFP_KERNEL);

	if (IS_ERR(my_device_1))
		goto fail;

	my_device_1->size = BUFFER;

	if (IS_ERR(my_device_1->message))
		goto fail;
    
	cl = class_create(THIS_MODULE, CLS);

	if (IS_ERR(cl))
		goto fail;
	    
	dev = device_create(cl, NULL, device_number, NULL, DEVICE);
	if (IS_ERR(dev))
		goto fail1;

	cdev_init(&my_device_1->char_device, &sops);

	ret = cdev_add(&my_device_1->char_device, device_number, 1);

	if (ret) {
		printk(KERN_ERR"unabe to add the character device\n");
		goto fail1;
	}
	return 0;

fail1:
	class_destroy(cl);
	kfree(my_device_1);
fail:
	unregister_chrdev_region(device_number, 1);
	return -1;
}

static void end_here(void)
{
	cdev_del(&my_device_1->char_device);
	device_destroy(cl, device_number);
	class_destroy(cl);
	unregister_chrdev_region(device_number, 1);
	kfree(my_device_1);
	printk(KERN_INFO"Going out of this bloody world\n");
		
}

MODULE_LICENSE("GPL");
module_init(start_here);
module_exit(end_here);

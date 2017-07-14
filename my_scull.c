#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/fcntl.h>

#include <linux/ioctl.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/list.h>

#include "my_scull.h"

static int ramdisk_size = RAMDISK_SIZE;
static int device_count = DEVICE_COUNT;
static int major_no;
static int minor_no = 0;

struct asp_mycdrv *my_devices;
struct class *my_cdrv_class;

module_param(device_count, int, S_IRUGO);
module_param(major_no, int, S_IRUGO);


static int my_module_open(struct inode *inode, struct file *filp)
{
	//declare vars here
	struct asp_mycdrv *temp_asp_mycdrv = container_of(inode->i_cdev, struct asp_mycdrv, dev);
	//logic here
	printk(KERN_INFO"Entered module_open\n");

	if(down_interruptible(&temp_asp_mycdrv->sem))
	{
		printk(KERN_ALERT"In module open. Unable to obtain lock !!!\n");
		return -1;
	}
	filp->private_data = temp_asp_mycdrv;
	up(&temp_asp_mycdrv->sem);
	return 0;
}

static int my_module_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO"Entered module_release\n");
	return 0;
}

static ssize_t my_module_read(struct file *filp, char __user *bufStoreUser, size_t bufCount, loff_t *curOffset)
{
	//declare vars here
	int return_val = 0, i;
	//struct asp_mycdrv *temp_asp_mycdrv = container_of(filp->private_data, struct asp_mycdrv, ramdisk);
	struct asp_mycdrv *temp_asp_mycdrv = (struct asp_mycdrv *)filp->private_data;
	char *ramdisk = temp_asp_mycdrv->ramdisk;
	printk(KERN_INFO"Entered module_read\n");
	//logic here

	if(down_interruptible(&temp_asp_mycdrv->sem))
	{
		printk(KERN_ALERT"In module read. Unable to obtain lock !!\n");
		return -1;
	}
	printk(KERN_INFO "bufCount is %d\n", bufCount);
	switch(temp_asp_mycdrv->dir_flag)
	{
		case 0:
			if( bufCount + *curOffset > ramdisk_size )
			{
				printk(KERN_ALERT "Requested data size too large\n");
				up(&temp_asp_mycdrv->sem);
				return -1;
			}
			return_val += bufCount - copy_to_user(bufStoreUser, ramdisk + *curOffset, bufCount);
			__put_user('\0', (char *)(bufStoreUser + bufCount));
			*curOffset += return_val;
			break;
		case 1:
			/*if( bufCount + *curOffset > ramdisk_size )
			{
				printk(KERN_ALERT "Requested data size too large\n");
				up(&temp_asp_mycdrv->sem);
				return -1;
			}*/
			return_val = bufCount;
			for(i = 0; i < bufCount; i++)
			{
				if( ramdisk + *curOffset - i < ramdisk )
				{
					__put_user('\0', (char *)(bufStoreUser + i));
					printk(KERN_INFO "%d lsser than ramdisk %d and %d\n", i, (int)ramdisk, (int)(ramdisk + *curOffset - i));
					break;
				}
				printk(KERN_INFO"%c", *(ramdisk+*curOffset-i));
				return_val -= copy_to_user(bufStoreUser + i, ramdisk + *curOffset - i, 1);
			}
			__put_user('\0', (char *)(bufStoreUser + i));
			*curOffset -= return_val;
			*curOffset = *curOffset < 0 ? 0 : *curOffset;
			break;
		default:
				printk(KERN_ALERT"Invalid option!! exiting\n");
				up(&temp_asp_mycdrv->sem);
				return -1;
	}
	printk(KERN_INFO "curOffset is %d\n", (int)*curOffset);
	up(&temp_asp_mycdrv->sem);
	return return_val;	
}

static ssize_t my_module_write(struct file *filp, const char __user *bufStoreUser, size_t bufCount, loff_t *curOffset)
{
	int return_val = 0, i;
	struct asp_mycdrv *temp_asp_mycdrv = (struct asp_mycdrv *)filp->private_data;
	char *ramdisk = temp_asp_mycdrv->ramdisk;
	//struct asp_mycdrv *temp_asp_mycdrv = container_of(filp->private_data, struct asp_mycdrv, ramdisk);
	printk(KERN_INFO"Entered module_write\n");

	if(down_interruptible(&temp_asp_mycdrv->sem))
	{
		printk(KERN_ALERT"In module write. Unable to obtain lock !!!\n");
		return -1;
	}
	switch(temp_asp_mycdrv->dir_flag)
	{
		case 0:
			if( bufCount + *curOffset > ramdisk_size )
			{
				printk(KERN_ALERT "Requested data size too large\n");
				up(&temp_asp_mycdrv->sem);
				return -1;
			}
			return_val += bufCount - copy_from_user(ramdisk + *curOffset, bufStoreUser, bufCount);
			*curOffset += return_val;
			break;
		case 1:
			/*if( bufCount + *curOffset > ramdisk_size )
			{
				printk(KERN_ALERT "Requested data size too large\n");
				up(&temp_asp_mycdrv->sem);
				return -1;
			}*/
			return_val = bufCount;
			for(i = 0; i < bufCount; i++)
			{
				if( ramdisk + *curOffset - i < ramdisk )
				{
					__put_user('\0', (char *)(bufStoreUser + i));
					printk(KERN_INFO "%d lsser than ramdisk %d and %d\n", i, (int)ramdisk, (int)(ramdisk + *curOffset - i));
					break;
				}
				printk(KERN_INFO"%c", *(ramdisk+*curOffset-i));
				return_val -= copy_from_user(ramdisk + *curOffset - i, bufStoreUser + i, 1);
			}
			*curOffset -= return_val;
			*curOffset = *curOffset < 0 ? 0 : *curOffset;
			break;
		default:
				printk(KERN_ALERT"Invalid option!! exiting\n");
				up(&temp_asp_mycdrv->sem);
				return -1;
	}
	up(&temp_asp_mycdrv->sem);
	return return_val;
}


static loff_t my_module_seek(struct file *filp, loff_t curOffset, int orig)
{
	loff_t temp_offset;
	struct asp_mycdrv *temp_asp_mycdrv = (struct asp_mycdrv *)filp->private_data;
	printk(KERN_INFO "Entered module seek\n");
	if( down_interruptible(&temp_asp_mycdrv->sem) )
	{
		printk(KERN_ALERT"Error obtaining lock in module seek!!\n");
		return -1;
	}
	switch(orig)
	{
		case SEEK_SET: temp_offset = curOffset;
				break;
		case SEEK_CUR:  temp_offset = filp->f_pos + curOffset;
				break;
		case SEEK_END:  temp_offset = ramdisk_size + curOffset;
				break;
		default:       printk(KERN_ALERT"Invalid Entry\n");
				up(&temp_asp_mycdrv->sem);
				return -EINVAL;
	}
	temp_offset = temp_offset < 0 ? 0 : temp_offset;
	temp_offset = temp_offset > ramdisk_size ? ramdisk_size : temp_offset;
	filp->f_pos = temp_offset;
	printk(KERN_INFO "f_pos is %d\n", (int)filp->f_pos);
	up(&temp_asp_mycdrv->sem);
	return temp_offset;
}

static long my_module_ioctl(struct file *filp, unsigned ioctl_num, unsigned long ioctl_param)
{
	int err = 0;
	int retval; 
	struct asp_mycdrv *temp_asp_mycdrv = (struct asp_mycdrv *)filp->private_data;
	if(down_interruptible(&temp_asp_mycdrv->sem))
	{
		printk(KERN_ALERT"Error obtaining lock in module ioctl!!\n");
		return -1;
	}
	/* * extract the type and number bitfields, and don't decode * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok() */
	if (_IOC_TYPE(ioctl_num) != SCULL_IOC_MAGIC)
	{
		printk(KERN_ALERT"Invalid command in module ioctl!!\n");
		up(&temp_asp_mycdrv->sem);
		return -ENOTTY;
	}
	if (_IOC_NR(ioctl_num) > SCULL_IOC_MAXNR)
	{
		printk(KERN_ALERT"Invalid number in module ioctl!!\n");
		up(&temp_asp_mycdrv->sem);
		return -ENOTTY;
	}
	/* * the direction is a bitmask, and VERIFY_WRITE catches R/W * transfers. 
	`Type' is user-oriented, while * access_ok is kernel-oriented, 
	so the concept of "read" and * "write" is reversed */
	if (_IOC_DIR(ioctl_num) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)ioctl_param, _IOC_SIZE(ioctl_num));
	else if (_IOC_DIR(ioctl_num) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)ioctl_param, _IOC_SIZE(ioctl_num));
	if (err)
	{
		printk(KERN_ALERT"Error accessing user space memory!!\n");
		up(&temp_asp_mycdrv->sem);
		return -EFAULT;
	}
	switch(ioctl_num)
	{
		case ASP_CHGACCDIR: retval = temp_asp_mycdrv->dir_flag;
				    __get_user(temp_asp_mycdrv->dir_flag, (int *)ioctl_param);
				    break;
		default:	   printk(KERN_ALERT"Invalid argument to module ioctl!!\n");
				   up(&temp_asp_mycdrv->sem);
				   return -1;
	}
	up(&temp_asp_mycdrv->sem);
	return retval;
}

static struct file_operations fops = 
{
	.open		=	my_module_open,
	.release	=	my_module_release,
	.read		=	my_module_read,
	.write		=	my_module_write,
	.llseek		=	my_module_seek,
	.unlocked_ioctl	=	my_module_ioctl, 
};

static int my_module_init(void)
{
	//declare variables here
	dev_t dev_no;
	int return_val = 0, i;
	struct asp_mycdrv *track_head;
	struct device *device_ptr;
	//start logic here
	printk(KERN_INFO"Entered module_init\n");
	if(!major_no)
	{
		printk(KERN_INFO "Major_No not provided as a parameter, assigning using alloc\n");
		return_val = alloc_chrdev_region(&dev_no, minor_no, device_count, "mycdrv");
		major_no = MAJOR(dev_no);
	}
	else
	{
		dev_no = MKDEV(major_no, minor_no);
		return_val = register_chrdev_region(dev_no, device_count, "mycdrv");

	}
	if( return_val )
	{
		printk(KERN_ALERT "Error allocating and creating device no.\n");
		return -ENOMEM;
		//call a cleanup function here.
	}

	my_cdrv_class = class_create(THIS_MODULE, "my_cdrv_class");
	if( !my_cdrv_class )
	{
		printk(KERN_ALERT "Error creating struct class pointer\n");
		return -1;
	}
	for(i = 0; i < device_count; i++)
	{
		my_devices = kmalloc(sizeof(struct asp_mycdrv), GFP_KERNEL);
		memset(my_devices, '\0', sizeof(my_devices));
		INIT_LIST_HEAD(&my_devices->list);
		if( i == 0 )
			track_head = my_devices;
		if( i != 0 )
			list_add_tail(&my_devices->list, &track_head->list);
/*
		my_devices[i].ramdisk = kmalloc(ramdisk_size, GFP_KERNEL);
		dev_no = MKDEV(major_no, minor_no + i);
		cdev_init(&my_devices[i].dev, &fops);
		my_devices[i].dev.owner = THIS_MODULE;
		my_devices[i].dev.ops = &fops;
		sema_init(&my_devices[i].sem, 1);
		return_val = cdev_add(&my_devices[i].dev, dev_no, 1);
		if( return_val )
		{
			printk(KERN_ALERT "Error adding device to /proc/devices\n");
			return -1;
		}
		device_ptr = device_create(my_cdrv_class, NULL, dev_no, NULL, "%s_%d", DEVICE_NAME, i);
		if( device_ptr <= 0 )
		{
			printk(KERN_ALERT "Error creating device file in /dev\n");
			return -1;
		}
*/
		my_devices->ramdisk = kmalloc(ramdisk_size, GFP_KERNEL);
		memset(my_devices->ramdisk, '\0', ramdisk_size);
		dev_no = MKDEV(major_no, minor_no + i);
		cdev_init(&my_devices->dev, &fops);
		my_devices->dev.owner = THIS_MODULE;
		my_devices->dev.ops = &fops;
		sema_init(&my_devices->sem, 1);
		return_val = cdev_add(&my_devices->dev, dev_no, 1);
		if( return_val )
		{
			printk(KERN_ALERT "Error adding device to /proc/devices\n");
			return -1;
		}
		device_ptr = device_create(my_cdrv_class, NULL, dev_no, NULL, "%s_%d", DEVICE_NAME, i);
		printk(KERN_INFO "This is device %s_%d and my address is %ld\n", DEVICE_NAME, i, (long)my_devices);
		if( device_ptr <= 0 )
		{
			printk(KERN_ALERT "Error creating device file in /dev\n");
			return -1;
		}
	}
	printk(KERN_INFO "Device initialization completed\n");
	return return_val;	
}


static void my_module_exit(void)
{
	//declare vars here
	int i;
	dev_t dev_no1 = MKDEV(major_no, minor_no), dev_no2;
	//logic here
	printk(KERN_INFO"Entered module_exit\n");
	if(my_devices)
	{
		for(i = device_count - 1; i >= 0; i--)
		{
			printk(KERN_INFO "Entered loop in exit %d with add %ld\n", i, (long)my_devices);
			dev_no2 = MKDEV(major_no, minor_no + i);
			device_destroy(my_cdrv_class, dev_no2);
			cdev_del(&my_devices->dev);
			kfree(my_devices->ramdisk);
			my_devices = list_entry(my_devices->list.prev, struct asp_mycdrv, list); 
		}
		class_destroy(my_cdrv_class);
		kfree(my_devices);
	}
	unregister_chrdev_region( dev_no1, device_count );
	printk(KERN_ALERT "Clean up done\n");
}
		

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_AUTHOR("Nikhil Nayak");
MODULE_DESCRIPTION("Assignment 4");
MODULE_LICENSE("GPL v2");

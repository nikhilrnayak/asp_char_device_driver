#ifndef MY_STRUCT_H
#define MY_STRUCT_H


//structure definitions

struct asp_mycdrv
{ 
	struct list_head list; 
	struct cdev dev; 
	char *ramdisk; 
	struct semaphore sem; 
	int devNo; 
	int dir_flag;
};


//constant definitions

#define DEVICE_NAME "mycdrv"

#define RAMDISK_SIZE (16*PAGE_SIZE)

#define DEVICE_COUNT 4

// ioctl definitions

/* Use 'n' as magic number */
#define SCULL_IOC_MAGIC  'n'

#define ASP_CHGACCDIR _IOW(SCULL_IOC_MAGIC,  1, int) 


#define SCULL_IOC_MAXNR 1

#endif

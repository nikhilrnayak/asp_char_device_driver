#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/mycdrv_"
#define SCULL_IOC_MAGIC  'n'
#define ASP_CHGACCDIR _IOW(SCULL_IOC_MAGIC,  1, int) 
char device[10];


void main(int argc, char **argv)
{
	int i, fd, seek_offset = 0, ret, dev_sel, dir;
	char c, write_buf[100], read_buf[100];
	printf("Enter the device you want to access:\n");
	printf("0> mycdrv_0\n1> mycdrv_1\n2> mycdrv_2\n3> mycdrv_3\n...\n");
	memset(device, '\0', 10);
	scanf(" %d", &dev_sel);
	printf("option selected is %d\n", dev_sel);
/*
	switch(dev_sel)
	{
		case 0: fd = open("/dev/mycdrv_0", O_RDWR);
			sprintf(DEVICE, "mycdrv_0", sizeof("mycdrv_0"));
			break;
		case 1: fd = open("/dev/mycdrv_1", O_RDWR);
			sprintf(DEVICE, "mycdrv_1", sizeof("mycdrv_1"));
			break;
		case 2: fd = open("/dev/mycdrv_2", O_RDWR);
			sprintf(DEVICE, "mycdrv_2", sizeof("mycdrv_2"));
			break;
		case 3: fd = open("/dev/mycdrv_3", O_RDWR);
			sprintf(DEVICE, "mycdrv_3", sizeof("mycdrv_3"));
			break;
		default: printf("Invalid option selected\n");
			 exit(-1);
	}
*/
	sprintf(device, "%s%d", DEVICE, dev_sel);
	fd = open(device, O_RDWR);
	if( fd == -1 )
	{
		printf("file %s doesn't exist or is being used by another process\n", DEVICE);
		exit(-1);
	}
	printf("Enter one of the following choice:\nr - read\nw - write\ni - ioctl\n");
	scanf(" %c", &c);
	switch(c)
	{
		case 'w': printf("Entere the offset position\n");
			  scanf(" %d", &seek_offset);
			  ret = lseek(fd, seek_offset, SEEK_CUR);
			  if( ret < 0 )
			  {
			  	printf("Error setting offset\n");
				exit(-1);
			  }
			  printf("Enter the data\n");
			  scanf(" %[^\n]", write_buf);
			  //printf("scanned data is %s\n", write_buf);
			  write(fd, write_buf, sizeof(write_buf));
			  break;
		case 'r': printf("Entere the offset position\n");
			  scanf(" %d", &seek_offset);
			  ret = lseek(fd, seek_offset, SEEK_CUR);
			  if( ret < 0 )
			  {
			  	printf("Error setting offset\n");
				exit(-1);
			  }
			  read(fd, read_buf, sizeof(read_buf));
			  printf("device data is %s\n", read_buf);
			  break;
		case 'i': printf("Enter the direction\n");
			  printf("0 - Forward\n1 - Reverse\n");
			  scanf(" %d", &dir);
			  ret = ioctl(fd, ASP_CHGACCDIR, &dir);
			  printf("Previous dir val is %d\n", ret);
			  if(ret == -1)
			  {
				printf("Error in ioctl\n");
			  }
			  break;
		default : printf("Invalid command\n");
			  break;		
	}
	close(fd);
}

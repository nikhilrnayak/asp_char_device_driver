1> To build use the command sudo make
2> To clean up use the command make clean
3> This creates dev files in /dev in the following format mycdrv_<dev no> ex: mycdrv_0, mycdrv_1
4> loading module sudo insmod my_scull.ko or with parameters like this sudo insmod my_scull.ko device_count=5. 
	Use similar format for major_no although the program assigns one if not given
5>	sudo chmod 777 /dev/my* to be able to run userapp as non root. or run as root using sudo ./userapp
6> I've included my own user app called test test_my_scull_drv.c
7> I've used 'n' as the magic number. if running with a different userapp, please change this to suit as needed.
8> To unload the module use sudo rmmod my_scull

including some example runs

mugen@ubuntu:~/Documents/test_c_progs/ASP/devdrivers/assign_4$ ./test_my_scull_drv
Enter the device you want to access:
0> mycdrv_0
1> mycdrv_1
2> mycdrv_2
3> mycdrv_3
...
0
option selected is 0
Enter one of the following choice:
r - read
w - write
i - ioctl
r
Entere the offset position
0
device data is THIS IS MYCDRV_0
Enter one of the following choice:
r - read
w - write
i - ioctl
i
Enter the direction
0 - Forward
1 - Reverse
1
Previous dir val is 0
Enter one of the following choice:
r - read
w - write
i - ioctl
r
Entere the offset position
0
device data is T
Enter one of the following choice:
r - read
w - write
i - ioctl
r
Entere the offset position
15
device data is 0_VRDCYM SI SIHT
Enter one of the following choice:
r - read
w - write
i - ioctl
r   
Entere the offset position
13
device data is VRDCYM SI SIHT
mugen@ubuntu:~/Documents/test_c_progs/ASP/devdrivers/assign_4$ rm test_my_scull_drv
mugen@ubuntu:~/Documents/test_c_progs/ASP/devdrivers/assign_4$ cc -o test_my_scull_drv test_my_scull_drv.c
test_my_scull_drv.c: In function ‘main’:
test_my_scull_drv.c:18:2: warning: incompatible implicit declaration of built-in function ‘memset’ [enabled by default]
mugen@ubuntu:~/Documents/test_c_progs/ASP/devdrivers/assign_4$ ./test_my_scull_drv 
Enter the device you want to access:
0> mycdrv_0
1> mycdrv_1
2> mycdrv_2
3> mycdrv_3
...
0
option selected is 0
Enter one of the following choice:
r - read
w - write
i - ioctl
r
Entere the offset position
15
device data is 0_VRDCYM SI SIHT
mugen@ubuntu:~/Documents/test_c_progs/ASP/devdrivers/assign_4$ ./test_my_scull_drv 
Enter the device you want to access:
0> mycdrv_0
1> mycdrv_1
2> mycdrv_2
3> mycdrv_3
...
1
option selected is 1
Enter one of the following choice:
r - read
w - write
i - ioctl
r
Entere the offset position
0
device data is 
mugen@ubuntu:~/Documents/test_c_progs/ASP/devdrivers/assign_4$ ./test_my_scull_drv 
Enter the device you want to access:
0> mycdrv_0
1> mycdrv_1
2> mycdrv_2
3> mycdrv_3
...
1
option selected is 1
Enter one of the following choice:
r - read
w - write
i - ioctl
w
Entere the offset position
0
Enter the data
THIS IS MYCDRV_1
mugen@ubuntu:~/Documents/test_c_progs/ASP/devdrivers/assign_4$ ./test_my_scull_drv 
Enter the device you want to access:
0> mycdrv_0
1> mycdrv_1
2> mycdrv_2
3> mycdrv_3
...
1
option selected is 1
Enter one of the following choice:
r - read
w - write
i - ioctl
r
Entere the offset position
0
device data is THIS IS MYCDRV_1

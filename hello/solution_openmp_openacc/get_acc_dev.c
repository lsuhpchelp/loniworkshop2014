#include <stdio.h> 
#include <accel.h> 

int main() { 
    int n; 
    acc_init(acc_device_nvidia); 
    n = acc_get_device_num(acc_device_nvidia); 
    printf("current device is:\t%d\n",n); 
} 


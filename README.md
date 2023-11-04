# Jakestering-Driver
Jakestering is an extremely simple gpio driver for the bcm2835 SoC on the raspberry pi zero v1.3 


DEPENDENCIES
  
    sudo apt-get update

    sudo apt-get install raspberrypi-kernel-headers

    to check if the kernel headers have been properly install:

    ls /lib/modules/$(uname -r)/build <- if this file exsist you are good to go to build jakestering_driver.ko


TO BUILD

    cd to_where_ever_you_installed_it/Jakestering-Driver

    run the make command

    ls and you should see a whole bunch of files but the only one you need is jakestering-driver.ko



IF YOU WANT THE DRIVER TO PRESIST THEN DO THIS:

    sudo mkdir /lib/modules/$(uname -r)/kernel/drivers/jakestering-driver

    sudo cp jakestering-driver.ko /lib/modules/6.1.21+/kernel/drivers/jakestering-driver/

    Now cd /etc/modules-load.d/

    edit the modudles.conf if there is one. If not make it.

    add the line jakestering-driver

    Now reboot

IF YOU DO NOT WANT THE DRIVER TO PRESIST THEN DO THIS:

    sudo insmod jakestering-driver.ko



TO SEE IF IT ALL WORKED:

    lsmod | grep jake in the terminal

    you can also check the /proc/ directory with ls /proc/ if you see jakestering_driver then it's installed



TO USE THE DRIVER YOU CAN:

  you can now echo "1,21,0" > /proc/jakestering-driver - to set pin 21 to output

  and echo "3,21,1" > /proc/jakestering-driver - to write pin 21 high

  you can also echo "0,21,0" > /proc/jakestering-driver - to read the value at pin 21
  
  then you can cat /proc/jakestering-driver - to see the value of the pin you just read

    First Digit : 0 = INPUT, 1 = OUPUT, 2 = READ, 3 = WRITE, 4 = PUD CONTROL
    Second Digit: 0 - 27 all the pins on the raspberry pi zero v1.3
    Thrid Digit : 0 = LOW / DISABLE PUD, 1 = HIGH / PULL DOWN, 2 = PULL UP



COMING SOON COMPANION LIBRARY "JAKESTERING" COMING SOON

*for c only fuck anything else, but you can make it if you want to...*


THIS ONLY WORKS FOR THE RASPBERRY PI ZERO v1.3!!! It could work for other boards if they have the bcm2835.

Email if you have any questions. Please make the subject jakestering_driver or jakestering.

Jkellum819@gmail.com

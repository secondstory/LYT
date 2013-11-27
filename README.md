# LYT

LYT is aa collaborative interactive ambient light fixture built using Intel's Galileo development board. It was developed for the Maker Faire Rome 2013 as a proof of concept on using the new Galileo board made by Intel.

This documents explains the architecture backing this project and how to compile it yourself. This is not an extensive step by step documentation but rather a collection of pointers to explain you how to do it yourself.

For a visual introduction to the project, you can have a look at this video on [the creators project website](http://thecreatorsproject.vice.com/the-makers-series/the-makers-lit-by-second-story).


## Lyt architecture 

### Introduction
This part is a high level description of the LYT architecture. Its intended purpose is to give the reader a first approach about which software is being run where and how the hardware is set up.

### Hardware description

On our current setup we are using 4 Galileo boards: 

* One runs as the server/wifi hotspot
* 3 are used to power the columns. 
* Each board has its own (Half  PCI-E) Intel N-135 Wifi Card and a Half to Full Height Mini PCI Express(PCI-E) Card Bracket Adapter to connect it to the board

We built 3 columns :

* Each contains 6 daisy chained strips of LED.
* LED strips are individually addressable, having 32 led/m using the WS2801 led controller (Arduino library on adafruit)
* One external  5V (45A)  power supply; each strip can be electrically connected to a 5V adaptor (same as Galileo board).
* 384 LEDs per column 
* The electrical power consumption of a column is around 75W 

### Networking description

The server board creates an open wifi network called LYT.
Each board connects to it, and gets its own fixed IP address (configured server side through MAC addresses)
Clients will connect to the wifi, request the http://lyt/ webpage and will be delivered with our custom UI.

Communication from phone clients to server is made through websockets. Communication between boards is made using OSC messages.

####Network setting
 
* WIFI Board : 192.168.0.10  (lyt)
* Board 1 : 192.168.0.11   mac:
* Board 2 : 192.168.0.12   mac: 
* Board 3 : 192.168.0.13   mac:
 


### Software description
#### Server board
The server board runs a couple of extra services than the stock installation :

* *hostapd* : for creating the access point
* *udhcpd* : Lightweight DHCP server
* *(busybox) dnsd* : used for DNS resolution of the http:://lyt address (was customized for acting as a captive portal, but not really working with iPhone so the customization was dropped)

On top of that we installed the libwebsockets library (compiled from source).

Finally the board runs an arduino sketch (compiled against the libwebsockets library) that act as a web-server and an OSC client forwarding touch information to the proper board.

#### Collumn board
Each board runs our Interactive drawing sketch, driving the LED strips. This is where the (OSC) touch message are translated into a meaningful visualization.




## How to set up the Galilleo build environment - Using a VM


### Prerequisites 

For this tutorial, we're going to use :

* Virtual Box 4.3.2
* Ubuntu 13-10-desktop 64bit


As a prerequisite download the files from their respective website. As not everyone has a linux installed on their machine, we're going to assume you're running this version of Ubuntu within a virtualized environment.



Install Virtual box, and create a new virtual machine, allocate 50GB of storage (or more/less according to what you want to do with it), and preferably try to enable the hardware acceleration, that you can find on the settings of the machine.
If when you start the machine for the first time it crashes almost immediatly, it's probably because you need to enable hardware virtulization on your machine. You do so by rebooting and changing the bios settings to enable VT-X and AMD-V, or if you're running windows 8, you need to reboot in advanced mode (see [this](http://www.makeuseof.com/tag/how-to-access-the-bios-on-a-windows-8-computer/)).

Once you've got the VM running, install Ubuntu on it. Once it's done, don't forget to install the guest tools from Virtual Box, they'll come handy. A good to have is to set up a shared folder between your host system and the linux running on the VM. Google [tutorials](http://navet.ics.hawaii.edu/~casanova/VirtualBoxUbuntuHowTo.html) for it.

### Setting up SD CARD

You'll need the following software:

* GParted
* p7zip-full


Plug the SD card in using an external USB reader, capture it using Virtual Box. The drive should appear in your virtual machine.
Use GParted to create two partition on the SDCard : the first one must be 100 MB in FAT32, the second one is the remaining space in ext3.

Download the SDCard image from Intel website (called : `LINUX_IMAGE_FOR_SD_Intel_Galileo_v0.7.5.7z`)
Extract it using p7zip : `7z e LINUX_IMAGE_FOR_SD_Intel_Galileo_v0.7.5.7z`
Copy the file on the new ext3 partition you created : `dd if=image-full-clanton.ext3 of=/dev/sdb2 bs=1024`
The process is going to take at least 20 minutes, be patient.

Edit the grub.conf file so that the first line says default 1 (instead of default 2). This way, the board will boot automatically on the partition you've created.
And then copy on the FAT32 partition the other files (`bzImage` and `/boot/grub/grub.conf`)
It is also the perfect moment for copying the content of the folder `srv_card` from our repo to the SD Card if you need those files (don't forget to `chmod +x` the files that should be executable).
The SD Card is now ready!


### Setting up the build environment

You need to download : 

* [Board Support Package Sources for Intel Quark ](https://communities.intel.com/community/makers/software/drivers)
* [Intel® Quark BSP Build Guide](https://communities.intel.com/docs/DOC-21882)


You can follow the Build Guide to set up your build environment. 
To compile your own library and software you need to follow the step 1. 2. and 6. of the guide (Setting up the environment and build the cross compiler tool chain)

### Running for the first time

when you want to boot on the sd-card on the first time, you may have to upgrade the firmware of the Galileo board. To do so follow the instruction on the getting started pdf coming with the software.
Setting up a serial connection using the (Jack) serial interface can be handy to debug booting problems. Connect a Jack-serial cable to the board, if needed use a USB to serial adaptor to connect it to your computer. Then install Putty and open a connection to the right COM port. For more details on the process of connection to the board using a serial connection, read the "upgrading manually the firmware" section in the Getting started doc. (You don't have to upgrade the firmware manually though)

The default user on the board is root, with no password.


## How to run LYT

### Compiling and running LYT (server side)


#### Libraries needed

* OSC
* libwebsockets (see below)
* WS2801
* Wifi fixes (see below) 

A couple of modifications are also required on the Arduino SDK to be able to compile LYT.

#### Setting up websockets and zlib libraries to the building environment

You need to copy the `libwebsockets.so` and `libz.so` files into `hardware\tools\x86\i586-poky-linux-uclibc\usr\lib` of your arduino folder.
On top of that you should also  set up the libwebsockets as a regular arduino library by copying the libwebsockets folder in the libraries folder of your arduino sdk.
You also need to copy the `libwebsockets.so` and `libwebsockets.so.4.0.0` on the sdcard in `/usr/lib folder` (you can do it from a VM accessing directly the sdcard or through SCP )

When all of this is done, you need to modify the arduino sdk build command sequence to link your software against the libraries you've added. To do so open the file `platform.win.txt` in `hardware\arduino\x86` and add the commands `-lz -lwebsockets` on the line that contains `-lm -lpthread`
Restart your Arduino IDE.

Please note: once this modification are done, you'll have to copy the libwebsockets on every board you're going to use, or no sketch will run (even those not actually using the library). You can also make a backup of the file and revert it when needed. 

#### Editing the wifi class
In `hardware\arduino\x86\libraries\WiFi\WiFi.cpp` you need to comment out the call to `init()` in the initialization of the class, and call it manually instead (or add it on the begin calls). This way the file `wpa_supplicant.conf` won't be deleted automatically at every start of the sketch (useful if you want to have the board connecting automatically to a network once and for all, and not hang for a while if rebooting a sketch) and the access point the server creates will stay live.


#### Setting up the services ####
Copy the content of the `srv_card` folder onto your sd card, and modify the files to suit your needs. You need for instance to modify the `udhcpd.conf` file to match your own mac adresses.
You may also register the `startAP` script to start at boot time (see linux documentation for that, you may modify `init.d/bootmisc.sh` or something along those line to start the script)
Another option is to run the script from the Touchserver sketch (a problem to that is in case of reboot of the script, the startAP script will crash the hotspot and the board will have to be rebooted)


### Setting up LYT column side

The setting up is straight forward, just think to change ip-addresses and/or network names in the sketches to match your current configuration.

## Dependencies

Here are the links towards the library we used for that project :

* [OSC](https://github.com/CNMAT/OSC)
* [libwebsockets](http://libwebsockets.org/)
* [Adafruit WS2801](https://github.com/adafruit/Adafruit-WS2801-Library)
* [hostapd](http://w1.fi/hostapd/)
* [busybox](http://busybox.net/)

## Conclusion

With these pointers you should be able to set up LYT yourself and hopefully tweak the code base to create some more impressive effects. Many improvement would have been possible on the code posted here, this project was intendeted to be a didactic proof of concept, not a fully optimized development. 
For instance, using a C library for the OSC handling would be smarter, but we wanted to play fair with the Arduino environment and avoid relying too much on the usage of non-arduino code.



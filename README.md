========================================================================================
RPiCluster Repository
========================================================================================
8-13-2013

Joshua Kiepert
joshkiepert@boisestate.edu

Overview
----------------------------------------------------------------------------------------
The RPiCluster project was started in late Spring 2013 in response to a need during my PhD
dissertation research. My research is currently focused on developing a novel data sharing system
for wireless sensor networks to facilitate in-network collaborative processing of sensor data. 
In the process of developing this system it became clear that perhaps the most expedient way to 
test many of the ideas was to create a distributed simulation rather than developing directly 
on the final target embedded hardware. Thus, I began developing a distributed simulation in which 
each simulation node would behave like a wireless sensor node (along with inherent communications 
limitations), and as such, interact with all other simulation nodes within a LAN. This approach 
provided true asynchronous behavior and actual network communication between nodes which enabled 
better emulation of real wireless sensor network behavior.

So, why I would want to build a Beowulf cluster using Raspberry Pis? The Raspberry Pi has a 
relatively slow CPU by modern standards. It has limited RAM, slow USB-based 10/100 Ethernet, and 
its operating system runs directly on a SD card. None of these “features” are ideal for a cluster 
computer! Well, there are several reasons. First, when your dissertation work requires the use of 
a cluster it is nice to ensure that there is one available all the time. Second, RPis provide a 
unique feature in that they have external low-level hardware interfaces for embedded systems use, 
such as I2 C, SPI, UART, and GPIO. This is very useful to electrical engineers (like myself) 
requiring testing of embedded hardware on a large scale. Third, having user-only access to a 
cluster (which is the case for most student-accessible systems) is fine if the cluster has all 
the necessary tools installed. If not however, you must then work with the cluster administrator 
to get things working. Thus, by building my own cluster I could directly outfit it with anything 
I might need. Finally, RPis are cheap! The RPi platform has to be one of the cheapest ways to 
create a cluster of 32 nodes. The cost for an RPi with an 8GB SD card is ~$45. For comparison, 
each node in one of the clusters available to students here at BSU, was about $1,250. So, for not 
much more than the price of one PC-based node, I could create a 32 node Raspberry Pi
cluster!

Update
-----------------------
While the BeagleBone Black was not available when I started this project, I would have chosen it 
rather than the Raspberry Pi had it been available. It is the same cost once you include an SD 
card, but it has onboard 2GB of flash storage for the operating system. It also uses a 
Cortex-A9 ARM processor running at 1GHz.

This repository contains the EagleCAD v5.x design files, MPI test code, C test code, and
Bash scripts I used for developing and testing the RPiCluster. 

----------------------------------------------------------------------------------------
Resource Descriptions
----------------------------------------------------------------------------------------

Documentation
-----------------------
+ docs/Creating.a.Raspberry.Pi-Based.Beowulf.Cluster[BSU].pdf
	Original white paper on the development of the RPiCluster.

+ docs/Creating.a.Raspberry.Pi-Based.Beowulf.Cluster[EEWeb].pdf
	Shorter article written for EEWeb's Pulse Magazine
	http://www.eeweb.com/pulse/issue-106-2013

EagleCAD v5.x Resources
-----------------------
+ eagle/power-led:  
	PCB layout for RPiCluster power/led boards
+ eagle/case-layout:
	Plexiglas panel layout (panel measurements, hole patterns, etc)

C Resources
-----------------------
+ c/blink
	C program that utilizes the wiringPi library to drive the RGB LED on the Power/LED board.

MPI Resources
-----------------------
+ mpi/pblink
	C MPI program that blinks the RGB LEDs in various patterns on the RPiCluster.

Bash Script Resources
-----------------------
+ scripts/cexec
	Executes a program on multiple machines via SSH
+ scripts/cscp
	Copies a local file to multiple machines via SSH
+ scripts/cshutdown
	Powers down multiple machines via SSH
+ scripts/cssh-copy-id
	Copies SSH identity to multiple machines for passwordless SSH
+ scripts/rgb-blink
	Blinks the RGB LEDs on the Power/LED board (using file I/O)
+ scripts/rpitemp
	Gets the RPi's SoC temperature
+ ssh-everywhere.sh
	Handy script for creating an SSH session on many machines using tmux

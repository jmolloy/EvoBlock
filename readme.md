EvoBlock - EvoBlock Simulator
========================================

What is EvoBlock Simulator?
----------------------------------------
The EvoBlock simulator acts acts as a software simulator of the FPGA based evolution platform used in the University of York Computer Science Evolvable Hardware Module. The practical in question can be found [http://labbookpages.co.uk/teaching/evoHW/lab1.html](here).

The EvoBlockSim is single-threaded, but a PC is so much quicker than the Microblaze that it makes no difference.

Requirements
----------------------------------------
* The Simulator is api compatible with the Hardware solution, so you need your solution code.
* SConstruct is the build system for the simulator.

How to build
----------------------------------------
Checkout this repos

`cp exp1.c /path/to/repos`

Alternatively you could symlink it

`cd /path/to/repos`

`scons`

Running
---------------------------------------
The simulator output is identical to the serial output of the hardware solution. However, the ArrayConfig.cellConfigs[] may be fed into the EvoBlock differently than the EvoBlockSim (row-column or column-row? its untested).

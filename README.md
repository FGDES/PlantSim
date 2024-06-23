# PlantSim - Qt Based Simulations of Physicak Plants

## Rational

In this project, we provide a collection of prototypical DES
applications for educational purposes. At the current stage, there
is a simple elevator scenario, a bottling station and a configurable
factory. The simulations are coded in Qt/C++ and imitate the plant
behaviour on a physical level.

Note: the configurable factory, FlexFact, is he most involved of our
simulations. Detailed documentation of the available components
is provided at

http://www.rt.eei.uni-erlangen.de/FGdes/flexfact.html.


Note: the bottling station was part of in in-house benchmark with
extensive documentation in the separate report (PDF-file included)


## Overview

A FAUDES application simulates a physical plant and
displays it's state graphically. Some applications
allow for user interaction, i.e. clicking an operator 
button or inserting a workpiece. Typically, an application 
constitues a hybrid system in that it exhibits both 
continuous and discrete dynamics. However, the external 
behaviour of a FAUDES application is restricted to 
discrete events only. Thus, we may use a DES supervisor 
to control the application.

Technically, FAUDES applications use the libFAUDES 
SimpleNet device to communicate discrete events. 
The device is automatically configure by the *.dev 
file that comes with the respective application. 
Example supervisors are provided in the Examples folder. 
Some examples come as DESTool projects, other as
simfaudes simulations. To run an example, it must
be configured to use the appropriate SimpleNet device
that comes with the respective example. In simfaudes,
the commanline option "-d" is used for device
configuratuion. In DESTool, the simulator configuration
menu allows to specify a *.dev file. In either case,
the FAUDES application will report on successful
device configuration in it's event log.


## Build System

The build system is organised in analogy to libVIODES/DESTool.
We are currently testing with Mac OS (Qt 6.6.2) and Linux (Qt 5.x.
Windows should be fine too, however, it is tested less frequently.









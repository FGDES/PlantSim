========================================================

FAUDES Plant Simulations

========================================================


The long term objective is to provide a number of 
prototypical DES applications for educational purposes.

At the current stage, there is a simple elevator scenario, 
a small factory, a configurable factory, and a bottling station. 
They are coded in Qt/C++. However, we are not quite sure, 
whether this is the  way to go. Proposals and suggestions 
are highly wellcome.

Note: the configurable factory, FlexFact is an independant
project which only for convenience is bundled with the
application package. Documentation is provided at
http://www.rt.eei.uni-erlangen.de/FGdes/flexfact.html.

Note: the bottling station was part of in in-house
benchmark with extensive documentation in the separate
report (PDF-file included)


========================================================

Overview 

========================================================


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


========================================================

Notes 

========================================================

* The Mac OS X bundle was compiled with OS X 10.11; it 
  should be compatible back to 10.7. The laat distribution
  of the application package compatible back to 10.4 is 0.78.

* The Linux binaries were compiled with LSB compilers on
  Debian Wheezy. They should be compatible back to 2010.
  DESTool comes with more note on binary compatibility

* MS Windows bonaries have been compiled with Microsoft
  cl, version 19, as it comes with Visual Studio 2015.
  You may need to install the redistributable runtime
  provided by Microsoft ("vc_redist.x64.exe")












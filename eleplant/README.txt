========================================================

FAUDES Elevator Simulation

========================================================


This application simulates a simple elevator. Find the below
list of relevant events. For general information on FAUDES
applications, see the provided README file.

Note: the simplenet network name is composed from the hostname. 
Thus, you will need to edit the network id in the device configuration 
of the provided example controllers.


cabin

c_stp     stop motor
c_up      start motor upwards
c_down    start motor downwards 

c_arup    arrive at upper position 
c_lvup    leave upper position 
c_arlw    arrive at lower position 
c_lvlw    leave lower position 


lower door
         
ld_stp    stop motor
ld_open   start motor to open door
ld_close  start motor to close door

ld_arcl   reached closed position
ld_lvcl   leaving closed position
ld_arop   reached opened position
ld_lvop   leaving opened position
ld_lbbl   light barrier blocked
ld_lbrl   light barrier released


upper door
         
ud_stp    stop motor
ud_open   start motor to open door
ud_close  start motor to close door
ud_arcl   reached closed position
ud_lvcl   leaving closed position
ud_arop   reached opened position
ud_lvop   leaving opened position

ud_lbbl   light barrier blocked
ud_lbrl   light barrier released



operator

o_cbb     cabin button pressed
o_upb     upper button pressed
o_lwb     lower button pressed

o_uplon   illuminate upper button 
o_lwlon   illuminate lower button 
o_uploff  turn off light for upper button
o_uploff  turn off light for lower button

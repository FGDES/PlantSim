=====================================================

Simulation of a Flexible Manufaturing System

=====================================================

This factory simulation resembles the LRT lab setup
of a flexible manufacturing system. The lab setup has been 
designed by Hans Reger and Klaus Schmidt as a basis to evaluate 
methods for hierarchical control of discrete event systems; 
for details, see http://www.rt.eeit.uni/erlangen/FGdes 

In this simulation, we apply a supervisor that has been 
synthesised by Klaus Schmidt and Sebastian Perk in 2007 and that 
was validated with the laboratory model. Since the event naming 
scheme originally introduced for the lab experiment does not 
quite match the event naming scheme used by the factory 
simulator, we use the below correspondence and process 
the supervisors with global 'search and replace', plus some
additional tweaks.

To run the factory simulation in closed loop with the
supervisor
1) start the factory simulation
2) navigate to the initial state
   * rotate tables 1,2 and 3
   * move both rail transprt carts one position inwards
3) start the simplenet I/O-device from within the simulator
4) use the provided shell script "super_simplenet.sh" to run the
   supervisor in another terminal


The original supervisor is provided as "./lrtdata/*gen". It's 
conversion has been performed by the shell script "convert.sh",
using the sed commands "convert_*.sed". The resulting supervisors 
are located in the direcories "./super_simplenet" and "./super_modbus",
the former using the libFAUDES simplenet protocol and the
latter using a Modbus/TCP commection.


=====================================================
Events not supported by the simulator

Some events originally defined for the lab experiment 
are not supported by the factory simulator. They either
are not referred to by the supervisor, or will be
executed as logical events. Examples include

sfnr        
pu1lv-y    [and all other pusher position leave events]
cp1lv      [and many other workpiece leave events]

Supervisor components, that refer to unsupported "leave" events 
do so in a strict sequence, e.g. "lv position A" allways followed
by "ar position b". Thus, execution as logical events is fine.

pmX_mack   [processing machine "process complete" event]

This event is only provided by the simulator, not by the
laboratory setup. The corresponding automata have been 
adjusted to eait for pmX_mack befor they stop the
machine. This is purely cosmetic.

=====================================================
Duplicate events not supported by the simulator

The original supervisor design was based on plant component 
models that "borrow" sensor events from neighbour components. 
This modelling technique turned out particulary useful for  
conveyor belts, which on their own have no chance to figure 
whether they are empty or not, e.g. conveyor belt A will know 
it is empty when driving the workpiece towards belt B and 
when B eventually issues a workpiece arrive event.
Thus, the model of belt A refers to the "wpar" event of belt B.
To keep the number of formally shared events low, the "borrowed" 
event of the one component is introduced as a duplicate with a 
different name. Since sensor events are uncontrollable, the supervisor 
should accept them at any time they occur, including (almost) simultaneous 
occurence (which will be the case, since the respective events are 
actually only one event. 

For a signal-based connection via the Modbus/TCP protocoll,
one may start with the automatically generated device configuration
file and manualy add the duplicat events. Such a "tweaked" device 
configuration is provided in "super_modbus.dev".

For an event-based connection via the libFAUDES Simplenet protocoll,
things are more tricky. In the original supervisor, the duplicate
events have an "optional execution" status, i.e. when they
occur in a state where they are not accepted, they are silently ignored. 
This strictly requires the duplicate events to have different names, 
so one duplicate may be ignored while another one is executed. Hence
we cannot map the duplicate events to the original event by
simple renaming. We instead mimique the optional execution semantics
by the simulator configuration "super_simplenet.sim", which some additional 
generators that translate between events received via Simplanet to 
events provided to the supervisr.


List of events with corresponding duplicats

cb4wpar:  cb12wplv5  cb13wplv5
cb4wpar:  cb11wplv4  cb12wplv4
cb6wpar:  cb13wplv6  cb14wplv6
cb5wpar:  cb12wplv5  cb13wplv5
cb7wpar:  cb11wplv7  cb15wplv7  (optionally: cb16wplv7)
cb8wpar:  cb12wplv8  cb15wplv8  cb16wplv8
cb9wpar:  cb13wplv9  cb15wplv9  cb16wplv9
cb10wpar: cb16wplv10 cb14wplv10 (optionally: cb15wplv10)
cb11wpar: cb4wplv11  cb7wplv11
cb12wpar: cb3wplv12  cb4wplv12  cb5wplv12 cb8wplv12
cb13wpar: cb9wplv13  cb2wplv13  cb5wplv13 cb6wplv13 
cb14wpar: cb6wplv14  cb10wplv14
cb15wpar: cb7wplv15  cb8wplv15  cb9wplv15 
cb16wpar: cb9wplv16  cb10wplv16 (optionally cp8wplv16)

rc1wpar: cb15wplvrc1            (note: current the supervisors dont use rcXwpZZ events at all)
rc1wplv: rc1rdy
rc2wpar: cb16wplvrc2
rc2wplv: rc2rdy



====================================================
Sensor events, straight forward search&replace

[typos ahead: see the sed-script for the actual
renaming scheme]


sfwpar  <=>  sf_wpar
sfwplv  <=>  sf_wpar
sfr  <=>  sf_fdhome

pu1ar-y  <=>  ds_p1s+
pu1ar+y  <=>  ds_p1s-
pu2ar-y  <=>  ds_p2s+
pu2ar+y  <=>  ds_p2s-

cb1awpar  <=>  ds_p2wpar    
cb1awplv  <=>  ds_p2wplv
cb1bwpar  <=>  ds_p1wpar
cb1bwplv  <=>  ds_p1wplv

depwpar  <=>  xs1_wpar
depwplv  <=>  xs1_wplv

cb3wpar  <=>  cb1_wpar
cb3wplv  <=>  cb1_wplv
cb2wpar  <=>  cb2_wpar
cb2wplv  <=>  cb2_wplv
cb5wpar  <=>  cb3_wpar
cb5wplv  <=>  cb3_wplv

cb7wpar  <=>  cb4_wpar
cb7wplv  <=>  cb4_wplv
cb8wpar  <=>  cb5_wpar
cb8wplv  <=>  cb5_wplv
cb9wpar  <=>  cb6_wpar
cb9wplv  <=>  cb6_wplv
cb10wpar  <=>  cb7_wpar
cb10wplv  <=>  cb7_wplv

cb4wpar  <=>  pm1_wpar
cb4wplv  <=>  pm1_wplv

cb6wpar  <=>  pm2_wpar
cb6wplv  <=>  pm2_wplv

cb16wpar  <=>  rs_b2wpar
cb16wplv  <=>  rs_b2wplv
cb15wpar  <=>  rs_b1wpar
cb15wplv  <=>  rs_b1wplv

cb11wpar  <=>  rb1_wpar
cb11wplv  <=>  rb1_wplv
cb12wpar  <=>  rb2_wpar
cb12wplv  <=>  rb2_wplv
cb13wpar  <=>  rb3_wpar
cb13wplv  <=>  rb3_wplv
cb14wpar  <=>  rb4_wpar
cb14wplv  <=>  rb4_wplv

rt1ary  <=>  rb1_scw
rt1arx  <=>  rb1_sccw
rt2ary  <=>  rb2_scw
rt2arx  <=>  rb2_sccw
rt3ary  <=>  rb3_scw
rt3arx  <=>  rb3_sccw
rt4ary  <=>  rb4_scw
rt4arx  <=>  rb4_sccw

rc1wpar  <=>  xs2_wpar
rc1wplv  <=>  xs2_wplv
rc2wpar  <=>  xs3_wpar
rc2wplv  <=>  xs3_wplv

rts1ar1  <=>  rs_p1car
rts1ar2  <=>  rs_p2car
rts1ar3  <=>  rs_p4car
rts1ar4  <=>  rs_p4car
rts1ar5  <=>  rs_p8car
rts1ar6  <=>  rs_p9car

rts2ar1  <=>  rs_p1car    %% these are doublets, too
rts2ar2  <=>  rs_p2car
rts2ar3  <=>  rs_p4car
rts2ar4  <=>  rs_p4car
rts2ar5  <=>  rs_p8car
rts2ar6  <=>  rs_p9car

mh1ar+z  <=>  pm1_ps-
mh1ar-z  <=>  pm1_ps+
mh2ar+z  <=>  pm2_ps-
mh2ar-z  <=>  pm2_ps+


====================================================
Actuator events, straight forward search&replace


sfmv  <=>  sf_fdon
sfstp  <=>  sf_fdoff

pu1mv-y  <=>  ds_p1m+
pu1mv+y  <=>  ds_p1m-
pu1stp   <=>  ds_p1off
pu2mv-y  <=>  ds_p2m+
pu2mv+y  <=>  ds_p2m-
pu2stp   <=>  ds_p2off

cb1+x   <=>  ds_bm+
cb1-x   <=>  ds_bm-
cb1stp  <=>  ds_boff
cb2+y   <=>  cb2_bm-
cb2-y   <=>  cb2_bm+
cb2stp  <=>  cb2_boff
cb3+y   <=>  cb1_bm-
cb3-y   <=>  cb1_bm+
cb3stp  <=>  cb1_boff
cb4+x   <=>  pm1_bm+
cb4-x   <=>  pm1_bm-
cb4stp  <=>  pm1_boff
cb5+x   <=>  cb3_bm+
cb5-x   <=>  cb3_bm-
cb5stp  <=>  cb3_boff
cb6+x   <=>  pm2_bm+
cb6-x   <=>  pm2_bm-
cb6stp  <=>  pm2_boff
cb7+y   <=>  cb4_bm-
cb7-y   <=>  cb4_bm+
cb7stp  <=>  cb4_boff
cb8+y   <=>  cb5_bm-
cb8-y   <=>  cb5_bm+
cb8stp  <=>  cb5_boff
cb9+y   <=>  cb6_bm-
cb9-y   <=>  cb6_bm+
cb9stp  <=>  cb6_boff
cb10+y  <=>  cb7_bm-
cb10-y  <=>  cb7_bm+
cb10stp <=>  cb7_boff

cb11+x+y  <=>  rb1_bm-     
cb11-x-y  <=>  rb1_bm+
cb11stp   <=>  rb1_boff
cb12+x-y  <=>  rb2_bm+
cb12-x+y  <=>  rb2_bm-
cb12stp   <=>  rb2_boff
cb13+x-y  <=>  rb3_bm+
cb13-x+y  <=>  rb3_bm-
cb13stp   <=>  rb3_boff
cb14+x-y  <=>  rb4_bm+
cb14-x+y  <=>  rb4_bm-
cb14stp   <=>  rb4_boff

cb15+y  <=>  rs_b1m-
cb15-y  <=>  rs_b1m+
cb15stp <=>  rs_b1off
cb16+y  <=>  rs_b2m-
cb16-y  <=>  rs_b2m+
cb16stp <=>  rs_b2off

rt1mvy  <=>  rb1_rccw
rt1mvx  <=>  rb1_rcw
rt1stp  <=>  rb1_roff
rt2mvy  <=>  rb2_rcw
rt2mvx  <=>  rb2_rccw
rt2stp  <=>  rb2_roff
rt3mvy  <=>  rb3_rcw
rt3mvx  <=>  rb3_rccw
rt3stp  <=>  rb3_roff
rt4mvy  <=>  rb4_rcw
rt4mvx  <=>  rb4_rccw
rt4stp  <=>  rb4_roff

rts1+y  <=> rs_c1m+
rts1-x  <=>  rs_c1m-
rts1stp <=>  rs_c1off

rts2+x  <=>  rs_c2m+
rts2-x  <=>  rs_c2m-
rts2stp <=>  rs_c2off

mh1mv+z <=> pm1_pm-
mh1mv-z <=> pm1_pm+
mh1stp  <=> pm1_poff
mh2mv+z <=> pm2_pm-
mh2mv-z <=> pm2_pm+
mh2stp  <=> pm2_poff

d1run  <=> pm1_mon
d1stp  <=> pm1_moff
d2run  <=> pm2_mon
d2stp  <=> pm2_moff





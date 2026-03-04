SIMFAUDES=../../../libFAUDES_for_PlantSim/bin/simfaudes
SUPERVISOR=./super_modbus

echo "running lab experiment supervisor"
echo "using modbus device to operate on flexfact simulation"
echo
echo "press return to proceed, or ctrl-c to bail out"
read

SIMCOMMAND="$SIMFAUDES -dt 500 -d $SUPERVISOR/super_modbus.dev  $SUPERVISOR/super_modbus.sim  -q" 
echo $SIMCOMMAND  
$SIMCOMMAND &> /dev/null

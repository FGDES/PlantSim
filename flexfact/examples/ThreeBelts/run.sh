# Execute transport system supervisor with simplenet interface
#
# 

# Configure
SIMFAUDES=../../../libfaudes/bin/simfaudes
SIMFILE=transsuper.sim
DEVFILE=transsuper.dev

echo Transport System Supervisor
echo
echo This script runs a supervisor for the transport system
echo with a simplenet interfaceon to match the manufacturing system
echo simulation.
echo 
echo
echo Files
echo - supervisor         "supervisor.gen"
echo - executor conf      "transsuper.sim"
echo - device conf        "transsuper.dev"
echo
echo You may specify additional simulator options, eg 
echo \  \"-q\"  for quiet operation
echo \  \"-v\"  for verbose operation
echo
echo Press Enter to proceed or ctrl-C to bail out.
read


# perform simulation
$SIMFAUDES -d $DEVFILE $@ $SIMFILE
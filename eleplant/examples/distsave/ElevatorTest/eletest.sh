#/bin/sh

# faudes simulator executable
SIMFAUDES=../../../libviodes/libfaudes/bin/simfaudes


echo This script runs a test supervisor for the elevator plant

$SIMFAUDES -d ../elesuper.dev -la -dr eletest.sim 



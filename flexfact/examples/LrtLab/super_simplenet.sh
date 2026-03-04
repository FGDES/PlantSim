SIMFAUDES=../../../libfaudes/bin/simfaudes
SUPERVISOR=./super_simplenet

echo "running lab experiment supervisor"
echo "using simplenet device to operate on flexfact simulation"
echo
echo "press return to proceed, or ctrl-c to bail out"
read


$SIMFAUDES -dt 500 -d $SUPERVISOR/super_simplenet.dev  $SUPERVISOR/super_simplenet.sim   -q   &> /dev/null

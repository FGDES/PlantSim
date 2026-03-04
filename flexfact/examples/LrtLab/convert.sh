echo "This script will copy the original supervisor"
echo "from labsuper to super_simplenet and super_modbus." 
echo "Any existings files in these folders will be overwritten."
echo
echo "MAKE SURE YOU HAVE A BACKUP"
echo
echo "press return to proceed, or ctrl-c to bail out"
read

rm -rf super_simplenet/*
cp labdata/*.gen super_simplenet
cp super_simplenet.sim super_simplenet
cp super_simplenet.dev super_simplenet

for g in super_simplenet/*.gen; do
  echo processing $g
  sed  -f convert_simplenet.sed  -i $g
#  sed  -f convert_simplenet.sed  -i "" $g
done

rm -rf super_modbus/*
cp labdata/*.gen super_modbus
cp super_modbus.sim super_modbus
cp super_modbus.dev super_modbus

for g in super_modbus/*.gen; do
  echo processing $g
  sed  -f convert_modbus.sed  -i $g
#  sed  -f convert_modbus.sed  -i "" $g
done



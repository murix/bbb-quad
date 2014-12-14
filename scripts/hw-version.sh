
#strings /sys/bus/i2c/devices/0-0050/eeprom


          REV=$(hexdump -e '8/1 "%c"' /sys/bus/i2c/devices/0-0050/eeprom -s 14 -n 2)
       SERIAL=$(hexdump -e '8/1 "%c"' /sys/bus/i2c/devices/0-0050/eeprom -s 16 -n 12)
        MODEL=$(hexdump -e '8/1 "%c"' /sys/bus/i2c/devices/0-0050/eeprom -s 8 -n 4)

echo "model=$MODEL rev=$REV serial=$SERIAL "


  MACETH0=$(hexdump -v -e '1/1 "%02X" ":"' /proc/device-tree/ocp/ethernet@4a100000/slave@4a100200/mac-address | sed 's/.$//')
  MACETH1=$(hexdump -v -e '1/1 "%02X" ":"' /proc/device-tree/ocp/ethernet@4a100000/slave@4a100300/mac-address | sed 's/.$//')

echo "MACETH0=$MACETH0 MACETH1=$MACETH1"

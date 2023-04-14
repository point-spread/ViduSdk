#!/bin/bash
MACHINE_TYPE="$(uname -m)"
MACHINE_IS_64BIT='no'
if [ "${MACHINE_TYPE}" = 'amd64' -o "${MACHINE_TYPE}" = 'x86_64' ]; then
	MACHINE_IS_64BIT='yes'
fi

MACHINE_IS_ARM='no'
if [ "${MACHINE_TYPE}" = 'arm' -o "${MACHINE_TYPE}" = 'armv7l' -o "${MACHINE_TYPE}" = 'aarch' -o "${MACHINE_TYPE}" = 'aarch64' ]; then
	MACHINE_IS_ARM='yes'
fi

BASEDIR=$(dirname "$0") 
cd "$BASEDIR"

[ -f "/usr/lib/libftd3xx.so*" ] && sudo rm /usr/lib/libftd3xx.so*

if [  "${MACHINE_IS_ARM}" = 'yes' ]; then
echo "Arm architecture"
unzip -o d3xx_linux-arm-v8.zip
sudo cp d3xx_linux-arm-v8/libftd3xx.so /usr/lib/
sudo cp d3xx_linux-arm-v8/51-ftd3xx.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
rm -rf d3xx_linux-arm-v8
elif [ "${MACHINE_IS_64BIT}" = 'yes' ]; then
echo "x86_64 architecture"
unzip -o d3xx-driver.zip
sudo cp d3xx-driver/linux-x86_64/libftd3xx.so /usr/lib/
sudo cp d3xx-driver/linux-x86_64/libftd3xx.so.0.5.21 /usr/lib/
sudo cp d3xx-driver/linux-x86_64/51-ftd3xx.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
rm -rf d3xx-driver
else
echo "unKnown architecture"
fi

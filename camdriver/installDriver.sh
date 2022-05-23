unzip d3xx-driver.zip
sudo rm /usr/lib/libftd3xx.so
sudo cp d3xx-driver/linux-x86_64/libftd3xx.so /usr/lib/
sudo cp d3xx-driver/linux-x86_64/libftd3xx.so.0.5.21 /usr/lib/
sudo cp d3xx-driver/linux-x86_64/51-ftd3xx.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
rm -rf d3xx-driver

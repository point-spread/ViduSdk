#!/bin/bash
###
 # @Author: Kian Liu
 # @Date: 2022-05-23 17:42:39
 # @LastEditTime: 2022-06-09 12:40:40
 # @LastEditors: Kian Liu
 # @Description: 
 # @FilePath: /ViduSdk/install.sh
### 
BASEDIR=$(dirname "$0")
SDK_INSTALL_DIR="$1"
cd "$BASEDIR"
sudo chmod +x SDKbin/*
echo "set cam driver..."
cd camdriver
sudo chmod +x installDriver.sh
sudo ./installDriver.sh 1>/dev/null
cd -
sudo cp camdriver/53-psf-camera.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger

# echo "install libglfw3-dev libglm-dev..."
# sudo apt-get -y install libglfw3-dev libglm-dev 1>/dev/null
# sudo apt-get -y install libtbb2 1>/dev/null

if [ ! -z "$SDK_INSTALL_DIR" -a "$SDK_INSTALL_DIR" != " " ]; then
  echo SDK_INSTALL_DIR specified at ${SDK_INSTALL_DIR}
else
  SDK_INSTALL_DIR=${PWD}
fi

echo SDK_INSTALL_DIR ${SDK_INSTALL_DIR}

echo "start make and pack GenTLwrapper"
mkdir -p SDKcode/GenTLwrapper/build
cd SDKcode/GenTLwrapper/build
cmake ../ -DCMAKE_INSTALL_PREFIX="${SDK_INSTALL_DIR}"
make -j
make install
cd -
rm -r SDKcode/GenTLwrapper/build
echo "end make and pack GenTLwrapper"

echo "set SDK running env..." 
export DYVCAM_GENTL64_PATH=${PWD}/SDKlib

[ -f "${SDK_INSTALL_DIR}/Vidu_SDK_Api/SDKlib/libGenTLwrapper.so" ] && export DYVCAM_GENTL64_PATH="${SDK_INSTALL_DIR}/Vidu_SDK_Api/SDKlib"

echo DYVCAM_GENTL64_PATH $DYVCAM_GENTL64_PATH
echo "delete previous DYVCAM_GENTL64_PATH env variable"

SCREEN_PYTHONPATH="PSF_TEMP_PYTHONPATH=(\${PYTHONPATH//:/ })\nfor index in \${!PSF_TEMP_PYTHONPATH[@]}\ndo\n  if [[ \${PSF_TEMP_PYTHONPATH[\$index]} == *ViduSdk* ]]; then\n    PSF_TEMP_PYTHONPATH[\$index]=\"\"\n  fi\ndone\nPSF_TEMP_PYTHONPATH=(\${PSF_TEMP_PYTHONPATH[*]/})\nPYTHONPATH=\$(IFS=:; echo \"\${PSF_TEMP_PYTHONPATH[*]}\")"
if [ "$EUID" -ne 0 ]
  if [ `grep -c "PSF_TEMP_PYTHONPATH" ~/.bashrc` -eq '0' ]; then
    echo -e "${SCREEN_PYTHONPATH}" >> ~/.bashrc
  fi
  then sed -i '/export DYVCAM_GENTL64_PATH/d' ~/.bashrc
  echo "export DYVCAM_GENTL64_PATH=\"${DYVCAM_GENTL64_PATH}\"" >> ~/.bashrc
  sed -i '/export PYTHONPATH=$PYTHONPATH:\"${DYVCAM_GENTL64_PATH}/d' ~/.bashrc
  echo "export PYTHONPATH=\$PYTHONPATH:\"\${DYVCAM_GENTL64_PATH}/SDKpythonLib\"" >> ~/.bashrc
else
  if [ `sudo grep -c "PSF_TEMP_PYTHONPATH" /etc/bash.bashrc` -eq '0' ]; then
    sudo echo -e "${SCREEN_PYTHONPATH}" >> /etc/bash.bashrc
  fi
  echo 'installed with all users when execute with sudo, may relogin let install complete'
  sudo sed -i '/export DYVCAM_GENTL64_PATH/d' /etc/bash.bashrc
  sudo sh -c 'echo "export DYVCAM_GENTL64_PATH='"$DYVCAM_GENTL64_PATH"'">> /etc/bash.bashrc'
  sudo sed -i '/export PYTHONPATH=$PYTHONPATH:\"${DYVCAM_GENTL64_PATH}/d' /etc/bash.bashrc
  sudo sh -c 'echo "export PYTHONPATH=\$PYTHONPATH:\"\${DYVCAM_GENTL64_PATH}/SDKpythonLib\"" >> /etc/bash.bashrc'
fi

sudo sh -c 'echo "'"${DYVCAM_GENTL64_PATH}"'" > /etc/ld.so.conf.d/DYV_SDK.conf'
sudo ldconfig

YELLOW='\033[1;33m'
NC='\033[0m' # No Color
echo -e "${YELLOW} Do you need to install patches to obtain more accurate metadata? [y/N] Press 'y' within 3 seconds to install. ${NC}"
echo -e "${YELLOW} If it has already been installed, please ignore this message. If your system has been updated, it needs to be patched again. ${NC}"
read -n 1 -t 3 -r answer
if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
  sudo bash ./camdriver/patch-okulo-ubuntu-lts-hwe.sh
fi

if [ "$EUID" -ne 0 ]; then
  echo 'remember to execute "source ~/.bashrc"'
else
  echo "relogin for changes to take effect"
fi

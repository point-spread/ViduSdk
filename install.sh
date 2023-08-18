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

echo "intsall libglfw3-dev libglm-dev..."
sudo apt-get -y install libglfw3-dev libglm-dev 1>/dev/null
sudo apt-get -y install libtbb2 1>/dev/null

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

if [ "$EUID" -ne 0 ]
  then sed -i '/export DYVCAM_GENTL64_PATH/d' ~/.bashrc
  echo "export DYVCAM_GENTL64_PATH=\"${DYVCAM_GENTL64_PATH}\"" >> ~/.bashrc
  sed -i '/export PYTHONPATH=$PYTHONPATH:\"${DYVCAM_GENTL64_PATH}/d' ~/.bashrc
  echo "export PYTHONPATH=\$PYTHONPATH:\"\${DYVCAM_GENTL64_PATH}/SDKpythonLib\"" >> ~/.bashrc
else
  echo 'installed with all users when execute with sudo, may relogin let install complete'
  sudo sed -i '/export DYVCAM_GENTL64_PATH/d' /etc/bash.bashrc
  sudo sh -c  'echo "export DYVCAM_GENTL64_PATH=\"'"$DYVCAM_GENTL64_PATH"'\"">> /etc/bash.bashrc'
  sudo sed -i '/export PYTHONPATH=$PYTHONPATH:\"${DYVCAM_GENTL64_PATH}/d' /etc/bash.bashrc
  sudo sh -c  'echo "export PYTHONPATH=\$PYTHONPATH:\"\${DYVCAM_GENTL64_PATH}/SDKpythonLib\"" >> /etc/bash.bashrc'
fi

sudo sh -c 'echo "'"${DYVCAM_GENTL64_PATH}"'" > /etc/ld.so.conf.d/DYV_SDK.conf'
sudo ldconfig



if [ "$EUID" -ne 0 ]; then
 echo 'remember to execute "source ~/.bashrc"'
else
 echo "relogin for changes to take effect"
fi
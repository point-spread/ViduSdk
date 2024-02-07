#!/bin/bash
sudo cp camdriver/53-psf-camera.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger

export DYVCAM_GENTL64_PATH=${PWD}/SDKlib
if [ "$EUID" -ne 0 ]; then
    sed -i '/export DYVCAM_GENTL64_PATH/d' ~/.bashrc
    echo "export DYVCAM_GENTL64_PATH=${DYVCAM_GENTL64_PATH}" >> ~/.bashrc
    sed -i '/export PYTHONPATH=$PYTHONPATH:\"${DYVCAM_GENTL64_PATH}/d' ~/.bashrc
    echo "export PYTHONPATH=\$PYTHONPATH:\"\${DYVCAM_GENTL64_PATH}/SDKpythonLib\"" >> ~/.bashrc
else
    echo 'installed with all users when execute with sudo, may relogin let install complete'
    sudo sed -i '/export DYVCAM_GENTL64_PATH/d' /etc/bash.bashrc
    sudo sh -c 'echo "export DYVCAM_GENTL64_PATH='"${DYVCAM_GENTL64_PATH}"'" >> /etc/bash.bashrc'
    sudo sed -i '/export PYTHONPATH=$PYTHONPATH:\"${DYVCAM_GENTL64_PATH}/d' /etc/bash.bashrc
    sudo sh -c 'echo "export PYTHONPATH=\$PYTHONPATH:\"\${DYVCAM_GENTL64_PATH}/SDKpythonLib\"" >> /etc/bash.bashrc'
fi

sudo sh -c 'echo "'"${DYVCAM_GENTL64_PATH}"'" > /etc/ld.so.conf.d/DYV_SDK.conf'
sudo ldconfig

YELLOW='\033[1;33m'
NC='\033[0m' # No Color
echo -e "${YELLOW} Do you need to install patches to obtain more accurate metadata (y/N), default is not install ${NC}"
echo -e "${YELLOW} If it has already been installed, please ignore this message ${NC}"
read answer
if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
    bash ./camdriver/patch-okulo-ubuntu-lts-hwe.sh
fi

if [ "$EUID" -ne 0 ]; then
    echo 'remember to execute "source ~/.bashrc"'
else
    echo "relogin for changes to take effect"
fi

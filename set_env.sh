#!/bin/bash
sudo cp camdriver/53-psf-camera.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger

if [ ! "$(command -v adb)" ]; then
    echo "[Info] need to install adb!"
    sudo apt-get install android-tools-adb
fi

export DYVCAM_GENTL64_PATH=${PWD}/SDKlib
SCREEN_PYTHONPATH="PSF_TEMP_PYTHONPATH=(\${PYTHONPATH//:/ })\nfor index in \${!PSF_TEMP_PYTHONPATH[@]}\ndo\n  if [[ \${PSF_TEMP_PYTHONPATH[\$index]} == *ViduSdk* ]]; then\n    PSF_TEMP_PYTHONPATH[\$index]=\"\"\n  fi\ndone\nPSF_TEMP_PYTHONPATH=(\${PSF_TEMP_PYTHONPATH[*]/})\nPYTHONPATH=\$(IFS=:; echo \"\${PSF_TEMP_PYTHONPATH[*]}\")"
if [ "$EUID" -ne 0 ]; then
    if [ `grep -c "PSF_TEMP_PYTHONPATH" ~/.bashrc` -eq '0' ]; then
        echo -e "${SCREEN_PYTHONPATH}" >> ~/.bashrc
    fi
    sed -i '/export DYVCAM_GENTL64_PATH/d' ~/.bashrc
    echo "export DYVCAM_GENTL64_PATH=${DYVCAM_GENTL64_PATH}" >> ~/.bashrc
    sed -i '/export PYTHONPATH=$PYTHONPATH:\"${DYVCAM_GENTL64_PATH}/d' ~/.bashrc
    echo "export PYTHONPATH=\$PYTHONPATH:\"\${DYVCAM_GENTL64_PATH}/SDKpythonLib\"" >> ~/.bashrc
else
    if [ `sudo grep -c "PSF_TEMP_PYTHONPATH" /etc/bash.bashrc` -eq '0' ]; then
        sudo echo -e "${SCREEN_PYTHONPATH}" >> /etc/bash.bashrc
    fi
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
echo -e "${YELLOW} Do you need to install patches to obtain more accurate metadata? (y/N), default is not install. ${NC}"
echo -e "${YELLOW} If it has already been installed, please ignore this message. If your system has been updated, it needs to be patched again. ${NC}"
read answer
if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
    sudo bash ./camdriver/patch-okulo-ubuntu-lts-hwe.sh
fi

if [ "$EUID" -ne 0 ]; then
    echo 'remember to execute "source ~/.bashrc"'
else
    echo "relogin for changes to take effect"
fi

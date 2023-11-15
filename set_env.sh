#!/bin/bash
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

if [ "$EUID" -ne 0 ]; then
    echo 'remember to execute "source ~/.bashrc"'
else
    echo "relogin for changes to take effect"
fi

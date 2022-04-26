######################################
# INSTALL OPENCV ON UBUNTU OR DEBIAN #
######################################

# -------------------------------------------------------------------- |
#                       SCRIPT OPTIONS                                 |
# ---------------------------------------------------------------------|
OPENCV_VERSION='4.2.0'       # Version to be installed
OPENCV_CONTRIB='NO'          # Install OpenCV's extra modules (YES/NO)
# -------------------------------------------------------------------- |

# |          THIS SCRIPT IS TESTED CORRECTLY ON          |
# |------------------------------------------------------|
# | OS               | OpenCV       | Test | Last test   |
# |------------------|--------------|------|-------------|
# | Ubuntu 20.04 LTS | OpenCV 4.2.0 | OK   | 25 Apr 2020 |
# |----------------------------------------------------- |
# | Debian 10.2      | OpenCV 4.2.0 | OK   | 26 Dec 2019 |
# |----------------------------------------------------- |
# | Debian 10.1      | OpenCV 4.1.1 | OK   | 28 Sep 2019 |
# |----------------------------------------------------- |
# | Ubuntu 18.04 LTS | OpenCV 4.1.0 | OK   | 22 Jun 2019 |
# | Debian 9.9       | OpenCV 4.1.0 | OK   | 22 Jun 2019 |
# |----------------------------------------------------- |
# | Ubuntu 18.04 LTS | OpenCV 3.4.2 | OK   | 18 Jul 2018 |
# | Debian 9.5       | OpenCV 3.4.2 | OK   | 18 Jul 2018 |



# 1. KEEP UBUNTU OR DEBIAN UP TO DATE

 apt-get -y update
# apt-get -y upgrade       # Uncomment to install new versions of packages currently installed
# apt-get -y dist-upgrade  # Uncomment to handle changing dependencies with new vers. of pack.
# apt-get -y autoremove    # Uncomment to remove packages that are now no longer needed


# 2. INSTALL THE DEPENDENCIES

# Build tools:
apt-get install -y build-essential cmake

# GUI (if you want GTK, change 'qt5-default' to 'libgtkglext1-dev' and remove '-DWITH_QT=ON'):
apt-get install -y libgtkglext1-dev libvtk6-dev

# Media I/O:

# git clone https://chromium.googlesource.com/webm/libwebp
# cd libwebp
# ./autogen.sh
# ./configure --enable-static --enable-shared=no CFLAGS="-fPIC" CXXFLAGS="-fPIC"
# make -j $(grep -c "^processor" /proc/cpuinfo)
# make install
# cd ..

apt-get install -y zlib1g-dev libjpeg-dev libwebp-dev libpng-dev libtiff5-dev libjasper-dev \
                        libopenexr-dev libgdal-dev

# Video I/O:
apt-get install -y libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev \
                        libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev yasm \
                        libopencore-amrnb-dev libopencore-amrwb-dev libv4l-dev libxine2-dev

# Parallelism and linear algebra libraries:
apt-get install -y libtbb-dev libeigen3-dev

# Python:
apt-get install -y python-dev  python-tk  pylint  python-numpy  \
                        python3-dev python3-tk pylint3 python3-numpy flake8

# Java:
apt-get install -y ant default-jdk

# Documentation and other:
apt-get install -y doxygen unzip wget


# 3. INSTALL THE LIBRARY
file=${OPENCV_VERSION}.zip
if [ ! -f "$file" ]; then
      wget --no-check-certificate https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip
fi
rm OpenCV -rf 
rm opencv-${OPENCV_VERSION} -rf
unzip ${OPENCV_VERSION}.zip
mv opencv-${OPENCV_VERSION} OpenCV

if [ $OPENCV_CONTRIB = 'YES' ]; then
  wget --no-check-certificate https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip
  unzip ${OPENCV_VERSION}.zip && rm ${OPENCV_VERSION}.zip
  mv opencv_contrib-${OPENCV_VERSION} opencv_contrib
  mv opencv_contrib OpenCV
fi

cd OpenCV
mkdir build
cd build

if [ $OPENCV_CONTRIB = 'NO' ]; then
cmake -DBUILD_DOCS=off \
	-DBUILD_SHARED_LIBS=on \
      -DBUILD_FAT_JAVA_LIB=off \
      -DBUILD_TESTS=off \
      -DBUILD_TIFF=on \
      -DBUILD_JASPER=on \
      -DBUILD_JPEG=on \
      -DBUILD_PNG=on \
      -DBUILD_ZLIB=on \
      -DBUILD_OPENEXR=off \
      -DBUILD_opencv_apps=off \
      -DBUILD_opencv_calib3d=off \
      -DBUILD_opencv_contrib=off \
      -DBUILD_opencv_features2d=off \
      -DBUILD_opencv_flann=off \
      -DBUILD_opencv_gpu=off \
      -DBUILD_opencv_java=off \
      -DBUILD_opencv_legacy=off \
      -DBUILD_opencv_ml=off \
      -DBUILD_opencv_nonfree=off \
      -DBUILD_opencv_objdetect=off \
      -DBUILD_opencv_ocl=off \
      -DBUILD_opencv_photo=off \
      -DBUILD_opencv_python=off \
      -DBUILD_opencv_stitching=off \
      -DBUILD_opencv_superres=off \
      -DBUILD_opencv_ts=off \
      -DBUILD_opencv_video=off \
      -DBUILD_opencv_videostab=off \
      -DBUILD_opencv_world=off \
      -DBUILD_opencv_lengcy=off \
      -DBUILD_opencv_lengcy=off \
      -DWITH_1394=off \
      -DWITH_EIGEN=on \
      -DWITH_TBB=ON \
      -DWITH_FFMPEG=off \
      -DWITH_GIGEAPI=off \
      -DWITH_GSTREAMER=off \
      -DWITH_GTK=on \
      -DWITH_PVAPI=off \
      -DWITH_V4L=off \
      -DWITH_LIBV4L=off \
      -DWITH_CUDA=off \
      -DWITH_CUFFT=off \
      -DWITH_OPENCL=off \
      -DWITH_OPENCLAMDBLAS=off \
      -DWITH_OPENCLAMDFFT=off \
      -DCMAKE_PREFIX_PATH=/usr/local ..
fi

if [ $OPENCV_CONTRIB = 'YES' ]; then
cmake -DWITH_QT=ON -DWITH_OPENGL=ON -DFORCE_VTK=ON -DWITH_TBB=ON -DWITH_GDAL=ON \
      -DWITH_XINE=ON -DENABLE_PRECOMPILED_HEADERS=OFF \
      -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ..
fi

make -j $(grep -c "^processor" /proc/cpuinfo)
make install
ldconfig

cd ..
cd ..
# 4. EXECUTE SOME OPENCV EXAMPLES AND COMPILE A DEMONSTRATION

# To complete this step, please visit 'http://milq.github.io/install-opencv-ubuntu-debian'.

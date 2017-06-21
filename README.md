# Instantly Launchable Browser

## Introduction
 - This web browser is Qt-based and Webkit-based browser for a system with NVRAM.


## Tested environment
 - OS: Ubuntu 14.04.5 64bit

## Installation
### 1. Install libraries for Qt 5.4 and our browser.
 - $ sudo apt-get update   
 - $ sudo apt-get install libxcomposite-dev libxrender-dev libjpeg-dev libx11-dev libpng12-dev libxslt1-dev libxml2-dev gir1.2-glib-2.0 libglib2.0-dev libgstreamer0.10-dev icu-devtools libgl1-mesa-dev libxcb1-dev liblzma-dev libpcre3-dev libffi-dev liborc-0.4-dev libexpat1-dev libxcb-present-dev libxcb-dri3-dev libxcb-sync-dev libxshmfence-dev libglapi-mesa libxext-dev libxdamage-dev libxfixes-dev libx11-xcb-dev libxcb-glx0-dev libxcb-dri2-0-dev libxxf86vm-dev libdrm-dev libxau-dev libxdmcp-dev libcups2-dev libevdev-dev alsa-base fontconfig libfreetype6-dev libharfbuzz-dev libicu-dev libgif-dev libmtdev-dev openssl libegl1-mesa-dev libpulse-dev libmysqlclient-dev libodbc1 libsqlite3-dev libudev-dev zlib1g-dev

### 1. Download and install Qt 5.4 for Linux x64.
 - $ wget http://download.qt.io/archive/qt/5.4/5.4.2/qt-opensource-linux-x64-5.4.2.run  
 - $ sudo ./qt-opensource-linux-x64-5.4.2.run

### 2. Get files from this repository.
 - $ git clone https://github.com/KyusikKim/ILB

### 3. Copy a Qt configuration file
 - For using qt5 binaries instead of qt4 binaries, you must copy a configuration file.
 - $ sudo cp ILB/qt_config/default.conf /usr/share/qtchooser

### 4. Make a Makefile for this browser.
 - $ qmake

### 5. Compile and Install the browser.
 - $ cd ILB  
 - $ make	  
 - $ sudo make install

### 6. Run the browser.
#### with X libraies: you cannot use features of instantly launchable browser. 
 - $ ilb [URL_OF_DEFAULT_PAGE]
#### without X libraries (i.e. booting linux with console mode): you can use the features.
 - $ sudo ilb_fb [URL_FOR_DEFAULT_PAGE] 


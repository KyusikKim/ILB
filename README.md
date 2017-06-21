# Instantly Launchable Browser

## Introduction
This web browser is Qt-based and Webkit-based browser for a system with NVRAM.


## Tested environment
 - OS: Ubuntu 14.04.5 64bit

## Installation
### 1. Download and install Qt 5.4 for Linux x64.
 $ wget http://download.qt.io/archive/qt/5.4/5.4.2/qt-opensource-linux-x64-5.4.2.run  
 $ sudo ./qt-opensource-linux-x64-5.4.2.run

### 2. Get files from this repository.
 $ git clone https://github.com/KyusikKim/ILB

### 3. Copy a Qt configuration file
#### For using qt5 binaries instead of qt4 binaries, you must copy a configuration file.
 $ cp ILB/qt_config/default.conf /usr/share/qtchooser

### 4. Make a Makefile for this browser.
 $ qmake

### 5. Compile and Install the browser.
 $ make  
 $ sudo make install

### 6. Run the browser.
#### with X libraies: you cannot use features of instantly launchable browser. 
 $ ilb [URL_OF_DEFAULT_PAGE]
#### without X libraries (i.e. booting linux with console mode): you can use the features.
 $ sudo ilb_fb [URL_FOR_DEFAULT_PAGE] 

# Contact us
## System Software Lab., Kwangwoon University, South Korea.
 Site: [sswlab.kw.ac.kr](https://sswlab.kw.ac.kr)


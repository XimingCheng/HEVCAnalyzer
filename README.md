HEVCAnalyzer
============
[![Build Status](https://api.travis-ci.org/XimingCheng/HEVCAnalyzer.png)](https://travis-ci.org/XimingCheng/HEVCAnalyzer)

Win7
![win32](https://cloud.githubusercontent.com/assets/2611722/4261031/818d5156-3b5a-11e4-9e2a-6b527ae4e3f9.png)

Ubuntu
![ubuntu](https://cloud.githubusercontent.com/assets/2611722/4212011/ab9afc00-3894-11e4-95c8-ca3febcffe64.png)

Linux Mint (old version)
![mint](https://f.cloud.github.com/assets/2611722/1792373/710f7022-699e-11e3-8415-6e2f1dfc60e5.jpg)

Introduction
------------

HEVCAnalyzer is a HEVC (H.265) stream and YUV file GUI Analyzer based on wxWidgets. The basic idea of the HEVCAnalyzer is building under Linux/Mac OS X/Windows for free. The core HEVC decoder is based on the HM open source project. The current HEVC decoder in the HEVCAnalyzer is HM 11.0. The goal of this project is just for fun and interest!

The project should be compiled with the C++11 flag enabled, some features of the code use the C++11 standard. The wxWidgets version of this project is 3.0.2, the version of wxWidgets in the older version of this project is 2.8.12 which retains some bugs. The newest code can be compiled with Visual Studio 2013, and the makefile under Linux will be updated soon. For Mac OS X, developer can build the code itself, I don't have money to buy a Mac and the application of HEVCAnalyzer does not fully test on Mac OS X.

Main features
-------------

* Support YUV file viewing
	* Viewing both 8bit and 10bit YUV file
	* Viewing the RGB pixel value of the frame in HEX mode or normal mode
	* Viewing the Y/U/V single color component
	* Recognize the formated YUV width and height, and remember the width/height when open it second time
* Support the thumbnails viewing of the YUV file and HEVC stream file
* Support scaling the frame of YUV and stream file
* Frame Ruler shows the frame size
* Support HEVC stream file viewing
	* CU/PU/TU coding block structure viewing
	* PU type viewing
	* Tiles viewing (part support)
	* Motion Vector viewing
	* Frame bit allocation (soon)
	* POC/Frame decoding order
	* And Soon More!
	
Build with Visual Studio 2013
-----------------------------

Download wxWidgets 3.0.2 and wxsqlite3 source code, build them with config with DLL Debug/Release, config the Visual Studio include path and the lib path (add the wxWidgets and wxsqlite3 include lib path)

Open the VS sln file under the Bulid/Win, build the source code with F7 or Ctrl + F5

Older Version Build Under Linux(Ubuntu)
---------------------------------------

Prefer clang++, but you can use g++ to build this project

    sudo apt-get install libx11-dev libwxbase2.8-dev libwxgtk2.8-dev libwxsqlite3-2.8-dev
    cd ~
    git clone https://github.com/XimingCheng/HEVCAnalyzer.git
    cd ~/HEVCAnalyzer/Build/Linux
    make
    
Get to run:

    cd ~/HEVCAnalyzer/bin
    ./HEVCAnalyzer
    
Also, you can download the wxWidgets-2.8-12 source code to build, but you must notice the **setup.h** as same as the Windows Building

Older Version Build Under Windows
---------------------------------

Download wxWidgets-2.8-12 source code, you can build it with Visual Studio or other IDE

**Notice that you must change the wxWidgets-2.8-12 source code setup.h from：**

```C
#ifndef wxUSE_GRAPHICS_CONTEXT
#define wxUSE_GRAPHICS_CONTEXT 0
#endif
```

**To：**

```C
#ifndef wxUSE_GRAPHICS_CONTEXT
#define wxUSE_GRAPHICS_CONTEXT 1
#endif
```

**If you use mingw to build wxWidgets, you must set `USE_GDIPLUS=1`**

Build the wxWidgets source code you can located the build dir the source dir:

    mingw32-make -f makefile.gcc MONOLITHIC=0 SHARED=1 UNICODE=1 USE_GDIPLUS=1  BUILD=release

You may change the BUILD mode to debug:

    mingw32-make -f makefile.gcc MONOLITHIC=0 SHARED=1 UNICODE=1 USE_GDIPLUS=1  BUILD=debug

then you have to download the wxsqlite3 source code, and build it with mingw

    mingw32-make -f makefile.gcc WX_VERSION=28 WX_SHARED=1 WX_MONOLITHIC=0 WX_UNICODE=1 WX_DEBUG=0 WX_DIR=/e/Project/wxMSW-2.8.12/
    mingw32-make -f makefile.gcc WX_VERSION=28 WX_SHARED=1 WX_MONOLITHIC=0 WX_UNICODE=1 WX_DEBUG=1 WX_DIR=/e/Project/wxMSW-2.8.12/

**NOTICE THAT** you must change `WX_DIR` to your own wxWidgets src dir

You can use the Code::Blocks project in HEVCAnalyzer/Build/Win/

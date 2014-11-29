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

HEVC/H.265 is the short name of the [High Efficiency Video Coding](http://en.wikipedia.org/wiki/High_Efficiency_Video_Coding) which is a video compression standard developed by the ISO/IEC Moving Picture Experts Group (MPEG) and ITU-T Video Coding Experts Group (VCEG). The difference between H.264/AVC and HEVC video coding standard can be found [here](http://iphome.hhi.de/wiegand/assets/pdfs/2012_12_IEEE-HEVC-Overview.pdf) . As the performance of the new HEVC standard is much better than former H.264/AVC, more and more devices and movies will adopt this standard soon.

HEVCAnalyzer is a HEVC (H.265) stream and YUV file GUI Analyzer based on wxWidgets which can give video developer a brief view of the video coding structures and video coding params. The project is launched under the interest which wish leading a free video coding analyzer for our researchers and video coding developers.

The program can be built on Windows Linux or Mac OS X, but the program does not fully test on MacOS X. I am trying to optimize the re-drawing efficiency as current the program re-drawing is very slow!

Main features
-------------

* Support YUV file
	* Both 8bit and 10bit YUV file
	* RGB pixel value of the frame in HEX mode or normal mode
	* Y/U/V single color component
	* Recognize the formated YUV width and height, and remember the width/height when open it second time
* Support the thumbnails of the YUV file and HEVC stream file
* Support scaling the frame of YUV and stream file in different size
* Frame Ruler shows the frame size
* Support HEVC stream file
	* CU/PU/TU coding block structure
	* PU type viewing
	* Tiles viewing (partly support)
	* Motion Vector
	* Frame bit allocation (soon)
	* POC/Frame decoding order
	* And Soon More!
	
Dependency
----------

Windows

	wxWidgets 3.0.2
	wxsqlite3 (build with wx30)

Linux

	libx11-dev libwxbase3.0-dev libwxgtk3.0-dev
	libwxsqlite3-3.0-dev (this can be found on Debian testing)

Complier

	Visual Studio 2013
	clang++ with -std=c++11

Build with Visual Studio 2013
-----------------------------

Download wxWidgets 3.0.2 and wxsqlite3 source code, build them with config with DLL Debug/Release, config the Visual Studio include path and the lib path (add the wxWidgets and wxsqlite3 include lib path)

Open the VS sln file under the Bulid/Win, build the source code with F7 or Ctrl + F5

Older Version Build Under Linux(Ubuntu)
---------------------------------------

Prefer `clang++`, but you can use g++ to build this project

if you want to build the project with `gcc`, you can chang to `CXX = g++` in the makefile of this project.

    sudo apt-get install libx11-dev libwxbase2.8-dev libwxgtk2.8-dev libwxsqlite3-2.8-dev
    cd ~
    git clone https://github.com/XimingCheng/HEVCAnalyzer.git
    cd ~/HEVCAnalyzer/Build/Linux
    make
    
Get to run:

    cd ~/HEVCAnalyzer/bin
    ./HEVCAnalyzer
    
Also, you can download the wxWidgets-2.8-12 source code to build, but you must notice the **setup.h** in the source should `#define wxUSE_GRAPHICS_CONTEXT 1`

Older Version Build Under Windows
---------------------------------

Download wxWidgets-2.8-12 source code, you can build it with Visual Studio or other IDE

**Notice that you must change setup.h in the wxWidgets-2.8-12 source code from：**

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

Contact
-------

Email: ximingc@mail.ustc.edu.cn
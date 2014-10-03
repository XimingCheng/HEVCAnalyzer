HEVCAnalyzer
============
[![Build Status](https://api.travis-ci.org/XimingCheng/HEVCAnalyzer.png)](https://travis-ci.org/XimingCheng/HEVCAnalyzer)

HEVC stream and YUV file GUI Analyzer based on wxWidgets

Ubuntu
![ubuntu](https://cloud.githubusercontent.com/assets/2611722/4212011/ab9afc00-3894-11e4-95c8-ca3febcffe64.png)

Linux Mint (old version)
![mint](https://f.cloud.github.com/assets/2611722/1792373/710f7022-699e-11e3-8415-6e2f1dfc60e5.jpg)

Win7
![win32](https://cloud.githubusercontent.com/assets/2611722/4261031/818d5156-3b5a-11e4-9e2a-6b527ae4e3f9.png)

Build Under Linux(Ubuntu)
-------------------------

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

Build Under Windows
-------------------

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

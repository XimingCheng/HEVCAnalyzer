HEVCAnalyzer
============
[![Build Status](https://api.travis-ci.org/XimingCheng/HEVCAnalyzer.png)](https://travis-ci.org/XimingCheng/HEVCAnalyzer)

HEVC stream and YUV file GUI Analyzer based on wxWidgets

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

**If you use mingw to build wxWidgets, you must set `-USE_GDIPLUS=1`**

You can use the Code::Blocks project in HEVCAnalyzer/Build/Win/

Linux Mint
![linux](https://f.cloud.github.com/assets/2611722/1792373/710f7022-699e-11e3-8415-6e2f1dfc60e5.jpg)

Win7
![win32](https://f.cloud.github.com/assets/2611722/1792380/a197fdc2-699e-11e3-8219-b33c2b1e1599.jpg)

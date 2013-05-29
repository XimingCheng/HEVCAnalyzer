#ifndef HEVCODECAPP_H_INCLUDED
#define HEVCODECAPP_H_INCLUDED

#include "HEVCAnalyser.h"
#include "MainFrame.h"
#if defined(__UNIX__)
#include <X11/Xlib.h>
#endif

class HEVCodecApp : public wxApp
{
public:
#if defined(__UNIX__)
    HEVCodecApp()
    {
        XInitThreads();
    }
#endif
    bool OnInit();
};

DECLARE_APP(HEVCodecApp)

#endif // HEVCODECAPP_H_INCLUDED

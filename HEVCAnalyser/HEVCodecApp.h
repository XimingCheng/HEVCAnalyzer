#ifndef HEVCODECAPP_H_INCLUDED
#define HEVCODECAPP_H_INCLUDED

#include "HEVCAnalyser.h"
#include "MainFrame.h"

class HEVCodecApp : public wxApp
{
public:
    bool OnInit();
};

DECLARE_APP(HEVCodecApp)

#endif // HEVCODECAPP_H_INCLUDED

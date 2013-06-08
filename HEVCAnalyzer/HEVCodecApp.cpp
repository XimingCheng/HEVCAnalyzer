#include "HEVCodecApp.h"

IMPLEMENT_APP(HEVCodecApp)

bool HEVCodecApp::OnInit()
{
    wxFrame* frame = new MainFrame(NULL, wxID_ANY, wxT("HEVC Codec Stream Analyzer"),
                                 wxDefaultPosition, wxSize(800, 600));
    SetTopWindow(frame);
    frame->Show();
    return true;
}

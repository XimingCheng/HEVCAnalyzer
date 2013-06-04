#ifndef PICVIEWCTRL_H_INCLUDED
#define PICVIEWCTRL_H_INCLUDED

#include "HEVCAnalyser.h"

class PicViewCtrl : public wxControl
{
public:
    DECLARE_DYNAMIC_CLASS(PicViewCtrl);

    PicViewCtrl() {}
    PicViewCtrl(wxWindow* parent, wxWindowID id)
        : wxControl (parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE), m_pViewBitmap(NULL)
    { }
    void SetScale(const double dScale);
    //void SetSize(const wxSize& size) { m_CtrlSize = size; }
    void SetBitmap(wxBitmap& bitmap);
    void SetLCUSize(const wxSize& size);

private:
    void OnPaint(wxPaintEvent& event);

private:
    double     m_dScaleRate;
    wxSize     m_CtrlSize;
    wxSize     m_LCUSize;
    wxBitmap*  m_pViewBitmap;

    DECLARE_EVENT_TABLE();
};

#endif // PICVIEWCTRL_H_INCLUDED

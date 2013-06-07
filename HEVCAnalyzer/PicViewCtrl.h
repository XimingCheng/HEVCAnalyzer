#ifndef PICVIEWCTRL_H_INCLUDED
#define PICVIEWCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"

class PicViewCtrl : public wxControl
{
public:
    DECLARE_DYNAMIC_CLASS(PicViewCtrl);

    PicViewCtrl() {}
    PicViewCtrl(wxWindow* parent, wxWindowID id)
        : wxControl (parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
        m_bClearFlag(true), m_curLCUStart(-1, -1), m_curLCUEnd(-1, -1), m_iLCURasterID(-1)
    { }
    void SetScale(const double dScale);
    //void SetSize(const wxSize& size) { m_CtrlSize = size; }
    void SetBitmap(wxBitmap bitmap);
    void SetLCUSize(const wxSize& size);
    void SetClear(bool bClr = true) { m_bClearFlag = bClr; }

private:
    void OnMouseMove(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);
    void Render(wxDC& dc);
    int  GetCurLCURasterID(long x, long y);

private:
    bool       m_bClearFlag;
    double     m_dScaleRate;
    wxSize     m_CtrlSize;
    wxSize     m_LCUSize;
    wxBitmap   m_cViewBitmap;
    // Mouse moving LCU location
    wxPoint    m_curLCUStart;
    wxPoint    m_curLCUEnd;
    int        m_iLCURasterID;

    DECLARE_EVENT_TABLE();
};

#endif // PICVIEWCTRL_H_INCLUDED

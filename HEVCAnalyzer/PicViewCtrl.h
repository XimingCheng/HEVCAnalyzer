#ifndef PICVIEWCTRL_H_INCLUDED
#define PICVIEWCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"

class PicViewCtrl : public wxControl
{
public:
    DECLARE_DYNAMIC_CLASS(PicViewCtrl);

    PicViewCtrl() {}
    PicViewCtrl(wxWindow* parent, wxWindowID id, wxSimpleHtmlListBox* pList)
        : wxControl (parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
        m_bClearFlag(true), m_bFitMode(true), m_dScaleRate(1.0), m_dMinScaleRate(0.1), m_dMaxScaleRate(2.0), m_dFitScaleRate(1.0),
        m_dScaleRateStep(0.02), m_delta(-1, -1), m_curLCUStart(-1, -1), m_curLCUEnd(-1, -1), m_iLCURasterID(-1), m_pList(pList)
    { }
    void SetScale(const double dScale);
    //void SetSize(const wxSize& size) { m_CtrlSize = size; }
    void SetBitmap(wxBitmap bitmap);
    void SetLCUSize(const wxSize& size);
    void SetClear(bool bClr = true) { m_bClearFlag = bClr; }
    void CalMinMaxScaleRate();
    void CalFitScaleRate();
    bool GetFitMode() const { return m_bFitMode; }
    void SetFitMode(const bool b) { m_bFitMode = b; }

private:
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLButtonDown(wxMouseEvent& event);
    void OnMouseLButtonUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);
    void Render(wxDC& dc);
    void ChangeScaleRate(const double rate);
    int  GetCurLCURasterID(const double x, const double y);

private:
    bool                 m_bClearFlag;
    bool                 m_bFitMode;
    double               m_dScaleRate;
    double               m_dMinScaleRate;
    double               m_dMaxScaleRate;
    double               m_dFitScaleRate;
    double               m_dScaleRateStep;
    wxSize               m_CtrlSize;
    wxSize               m_LCUSize;
    wxBitmap             m_cViewBitmap;
    wxPoint              m_delta;
    // Mouse moving LCU location
    wxPoint              m_curLCUStart;
    wxPoint              m_curLCUEnd;
    int                  m_iLCURasterID;
    wxSimpleHtmlListBox* m_pList;

    DECLARE_EVENT_TABLE();
};

#endif // PICVIEWCTRL_H_INCLUDED

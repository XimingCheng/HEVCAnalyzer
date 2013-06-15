#ifndef RULERCTRL_H_INCLUDED
#define RULERCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"

class RulerCtrl : public wxControl
{
public:
    DECLARE_DYNAMIC_CLASS(RulerCtrl);

    RulerCtrl() {}
    RulerCtrl(wxWindow* parent, wxWindowID id, bool bV = false);

    bool IsVertical() const { return m_bVertical; }
    int  GetStartPos() const { return m_iStartPos; }
    void SetStartPos(const int start) { m_iStartPos = start; }

private:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);

    void Render(wxGraphicsContext *gc);

private:
    bool    m_bVertical;
    int     m_iStartPos;
    int     m_iEndPos;
    int     m_iStartValue;
    int     m_iValuePerUnit;
    double  m_dScaleRate;
    int     m_iLongMarkLen;
    int     m_iTextMarkLen;
    int     m_iRulerWidth;

    DECLARE_EVENT_TABLE();
};

#endif // RULERCTRL_H_INCLUDED

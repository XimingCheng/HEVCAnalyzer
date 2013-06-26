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
    int  GetStartPos() const { return m_dStartPos; }
    int  GetEndPos() const { return m_dEndPos; }
    int  GetStartValue() const { return m_iStartValue; }
    int  GetValuePerUnit() const { return m_iValuePerUnit; }
    int  GetLongMarkLen() const { return m_iLongMarkLen; }
    int  GetTextMarkLen() const { return m_iTextMarkLen; }
    int  GetRulerWidth() const { return m_iRulerWidth; }
    bool IsAdaptiveMarkLen() const { return m_bAdaptiveMarkLen; }
    void SetStartPos(const double start);
    void SetEndPos(const double end);
    void SetStartValue(const int start);
    void SetEndValue(const int end);
    void SetValuePerUnit(const int value);
    void SetLongMarkLen(const int len);
    void SetTextMarkLen(const int len);
    void SetScaleRate(const double rate);
    void SetTagValue(const int value);
    void SetAdaptiveMarkLen(const bool b) { m_bAdaptiveMarkLen = b; }

private:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);

    void Render(wxGraphicsContext *gc);
    void SetAdaptiveMarkLen();

private:
    bool       m_bVertical;
    double     m_dStartPos;
    double     m_dEndPos;
    int        m_iStartValue;
    int        m_iEndValue;
    int        m_iValuePerUnit;
    double     m_dScaleRate;
    int        m_iLongMarkLen;
    int        m_iTextMarkLen;
    int        m_iRulerWidth;
    int        m_iTagValue;
    bool       m_bAdaptiveMarkLen;

    DECLARE_EVENT_TABLE();
};

#endif // RULERCTRL_H_INCLUDED

#ifndef BITSBARCTRL_H_INCLUDED
#define BITSBARCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"
#include <deque>

struct BarData
{
    int       _value;
    int       _poc;
    SliceType _type;
};

class BitsBarCtrl : public wxControl
{
public:
    DECLARE_DYNAMIC_CLASS(BitsBarCtrl);

    BitsBarCtrl() {}
    BitsBarCtrl(wxWindow* parent, wxWindowID id);
    void SetEndValue(const int value) { m_iEndValue = value;  }
    void SetBarWinWidth(const int w) { m_iBarWinWidth = w; }
    void SetBarWinHeight(const int h) { m_iBarWinHeight = h; }
    void SetBarWidth(const int w) { m_iBarWidth = w; }
    void SetBarHeight(const int h) { m_iBarHeight = h; }
    void AddData(const deque<BarData>& data);
    void ClearData();

private:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);

    void Render(wxGraphicsContext *gc);

private:
    int            m_iReDrawStart  = -1;
    int            m_iReDrawEnd    = -1;
    int            m_iEndValue     = 30000;
    int            m_iBarWinWidth;
    int            m_iBarWinHeight;
    int            m_iBarWidth     = 30;
    int            m_iBarHeight;
    int            m_iBarGapWidth  = 5;
    int            m_iSelectedIdx  = -1;
    deque<BarData> m_dBitsData;

    DECLARE_EVENT_TABLE();
};

#endif // BITSBARCTRL_H_INCLUDED
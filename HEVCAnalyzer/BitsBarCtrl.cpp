#include "BitsBarCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(BitsBarCtrl, wxControl);

BEGIN_EVENT_TABLE(BitsBarCtrl, wxControl)
    EVT_PAINT(BitsBarCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(BitsBarCtrl::OnEraseBkg)
END_EVENT_TABLE()

BitsBarCtrl::BitsBarCtrl(wxWindow* parent, wxWindowID id)
    : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void BitsBarCtrl::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
    Render(gc);
}

void BitsBarCtrl::OnEraseBkg(wxEraseEvent& event)
{
}

void BitsBarCtrl::Render(wxGraphicsContext *gc)
{
    wxSize size = GetClientSize();
    m_iBarWinWidth = size.GetWidth();
    m_iBarWinHeight = size.GetHeight();
    gc->SetPen(*wxWHITE_PEN);
    gc->SetBrush(*wxWHITE_BRUSH);
    gc->DrawRectangle(0, 0, size.x, size.y);
    m_iBarHeight = m_iBarWinHeight - 10;
    for (int curIdx = 0; curIdx < m_dBitsData.size(); curIdx++)
    {
        int startx = curIdx * (m_iBarWidth + m_iBarGapWidth);
        BarData data = m_dBitsData[curIdx];
        gc->SetPen(*wxCYAN_PEN);
        gc->SetBrush(*wxCYAN_BRUSH);
        int height = (int)(((double)data._value / m_iEndValue) * m_iBarHeight);
        gc->DrawRectangle(startx, m_iBarHeight - height, m_iBarWidth, height);
    }
}

void BitsBarCtrl::AddData(const deque<BarData>& data)
{
    int start = m_dBitsData.size();
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        m_dBitsData.push_back(*it);
    }
    Refresh();
}

void BitsBarCtrl::ClearData()
{
    m_iReDrawStart = 0;
    m_iReDrawEnd = 0;
    m_dBitsData.clear();
    Refresh();
}
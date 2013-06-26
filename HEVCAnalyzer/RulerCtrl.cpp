#include "RulerCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(RulerCtrl, wxControl);

BEGIN_EVENT_TABLE(RulerCtrl, wxControl)
    EVT_PAINT(RulerCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(RulerCtrl::OnEraseBkg)
END_EVENT_TABLE()

RulerCtrl::RulerCtrl(wxWindow* parent, wxWindowID id, bool bV)
    : wxControl (parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE),
    m_bVertical(bV), m_dStartPos(24), m_dEndPos(0), m_iStartValue(0.0), m_iEndValue(0), m_iValuePerUnit(4),
    m_dScaleRate(1.0), m_iLongMarkLen(16), m_iTextMarkLen(64), m_iRulerWidth(24), m_iTagValue(-1),
    m_bAdaptiveMarkLen(true)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    if(m_bVertical)
    {
        SetMinSize(wxSize(m_iRulerWidth, -1));
        m_dStartPos = 0;
    }
    else
        SetMinSize(wxSize(-1, m_iRulerWidth));
}

void RulerCtrl::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
    if(m_bAdaptiveMarkLen)
        SetAdaptiveMarkLen();
    Render(gc);
}

void RulerCtrl::OnEraseBkg(wxEraseEvent& event)
{
}

void RulerCtrl::Render(wxGraphicsContext *gc)
{
    wxSize size = GetClientSize();
    gc->SetPen(*wxWHITE_PEN);
    gc->SetBrush(*wxWHITE_BRUSH);
    gc->DrawRectangle(0, 0, size.x, size.y);
    gc->SetPen(*wxGREY_PEN);
    if(m_bVertical)
    {
        gc->StrokeLine(size.x-1, 0, size.x-1, size.y);
        m_dEndPos = size.y;
    }
    else
    {
        gc->StrokeLine(m_iRulerWidth, size.y-1, size.x, size.y-1);
        m_dEndPos = size.x;
    }
    gc->SetFont(*wxSMALL_FONT, *wxBLACK);
    for(int i = 0; m_dStartPos + i*m_iValuePerUnit*m_dScaleRate < m_dEndPos; i++)
    {
        int lineLen = 7;
        int pixel   = i*m_iValuePerUnit;
        int value   = pixel + m_iStartValue;
        if(value > m_iEndValue) break;
        double step = m_dStartPos + pixel*m_dScaleRate;
        if(!m_bVertical && step < m_iRulerWidth) continue;
        if(value % m_iLongMarkLen == 0)
            lineLen += 4;
        if(value % m_iTextMarkLen == 0)
        {
            wxString v;
            v.Printf(_T("%d"), value);
            double w, h, d, e;
            gc->GetTextExtent(v, &w, &h, &d, &e);
            if(m_bVertical)
            {
                int y = ((i == 0) ? step+w/2+3 : step+w/2);
                gc->DrawText(v, size.x-12-h, y, 3.14159/2);
            }
            else
                gc->DrawText(v, step-w/2, size.y-h-12);
        }
        if(m_bVertical)
            gc->StrokeLine(size.x-1, step, size.x-1-lineLen, step);
        else
            gc->StrokeLine(step, size.y-1, step, size.y-1-lineLen);
        if(value == m_iTagValue)
        {
            wxPen red(wxColor(255, 0, 0, 128));
            red.SetWidth(2);
            gc->SetPen(red);
            if(m_bVertical)
                gc->StrokeLine(size.x-1, step, 0, step);
            else
                gc->StrokeLine(step, size.y-1, step, 0);
            gc->SetPen(*wxGREY_PEN);
        }
    }
}

void RulerCtrl::SetStartPos(const double start)
{
    if(fabs(start - m_dStartPos) > MINDIFF)
    {
        m_dStartPos = start;
        Refresh();
    }
}

void RulerCtrl::SetEndPos(const double end)
{
    if(fabs(end - m_dEndPos) > MINDIFF)
    {
        m_dEndPos = end;
        Refresh();
    }
}

void RulerCtrl::SetStartValue(const int start)
{
    if(start != m_iStartValue)
    {
        m_iStartValue = start;
        Refresh();
    }
}

void RulerCtrl::SetEndValue(const int end)
{
    if(end != m_iEndValue)
    {
        m_iEndValue = end;
        Refresh();
    }
}

void RulerCtrl::SetValuePerUnit(const int value)
{
    if(value != m_iValuePerUnit)
    {
        m_iValuePerUnit = value;
        Refresh();
    }
}

void RulerCtrl::SetLongMarkLen(const int len)
{
    if(len != m_iLongMarkLen)
    {
        m_iLongMarkLen = len;
        Refresh();
    }
}

void RulerCtrl::SetTextMarkLen(const int len)
{
    if(len != m_iTextMarkLen)
    {
        m_iTextMarkLen = len;
        Refresh();
    }
}

void RulerCtrl::SetScaleRate(const double rate)
{
    if(fabs(rate - m_dScaleRate) > MINDIFF)
    {
        m_dScaleRate = rate;
        Refresh();
    }
}

void RulerCtrl::SetTagValue(const int value)
{
    if(value != m_iTagValue)
    {
        m_iTagValue = value;
        Refresh();
    }
}

void RulerCtrl::SetAdaptiveMarkLen()
{
    if(m_dScaleRate < 0.25)
    {
        m_iValuePerUnit = 8;
        m_iLongMarkLen  = 64;
        m_iTextMarkLen  = 256;
    }
    else if(m_dScaleRate > 2)
    {
        m_iValuePerUnit = 2;
        m_iLongMarkLen  = 8;
        m_iTextMarkLen  = 32;
    }
    else // default value
    {
        m_iValuePerUnit = 4;
        m_iLongMarkLen  = 16;
        m_iTextMarkLen  = 64;
    }
}

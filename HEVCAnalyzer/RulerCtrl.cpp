#include "RulerCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(RulerCtrl, wxControl);

BEGIN_EVENT_TABLE(RulerCtrl, wxControl)
    EVT_PAINT(RulerCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(RulerCtrl::OnEraseBkg)
    EVT_SIZE(RulerCtrl::OnSize)
END_EVENT_TABLE()

RulerCtrl::RulerCtrl(wxWindow* parent, wxWindowID id, bool bV)
    : wxControl (parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
    m_bVertical(bV), m_iStartPos(24), m_iEndPos(0), m_iStartValue(0), m_iValuePerUnit(4),
    m_dScaleRate(1.0), m_iLongMarkLen(16), m_iTextMarkLen(64), m_iRulerWidth(24)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    if(m_bVertical)
    {
        SetMinSize(wxSize(m_iRulerWidth, -1));
        m_iStartPos = 0;
    }
    else
        SetMinSize(wxSize(-1, m_iRulerWidth));
}

void RulerCtrl::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
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
        m_iEndPos = size.y;
    }
    else
    {
        gc->StrokeLine(m_iRulerWidth, size.y-1, size.x, size.y-1);
        m_iEndPos = size.x;
    }
    gc->SetFont(*wxSMALL_FONT, *wxBLACK);
    for(int i = 0; m_iStartPos+i*m_iValuePerUnit*m_dScaleRate < m_iEndPos; i++)
    {
        int lineLen = 7;
        int pixel   = i*m_iValuePerUnit;
        int value   = pixel + m_iStartValue;
        double step = m_iStartPos + pixel*m_dScaleRate;
        if(pixel % m_iLongMarkLen == 0)
            lineLen += 4;
        if(pixel % m_iTextMarkLen == 0)
        {
            wxString v;
            v.Printf(_T("%d"), value);
            double w, h, d, e;
            gc->GetTextExtent(v, &w, &h, &d, &e);
            if(m_bVertical)
            {
                int y = ((i == 0) ? step+w/2+3 : step+w/2);
                gc->DrawText(v, size.x-lineLen-h-1, y, 3.14159/2);
            }
            else
                gc->DrawText(v, step-w/2, size.y-lineLen-h-1);
        }
        if(m_bVertical)
            gc->StrokeLine(size.x-1, step, size.x-1-lineLen, step);
        else
            gc->StrokeLine(step, size.y-1, step, size.y-1-lineLen);
    }
}

void RulerCtrl::OnSize(wxSizeEvent& event)
{
    Refresh();
}

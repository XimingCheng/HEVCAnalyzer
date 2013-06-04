#include "PicViewCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(PicViewCtrl, wxControl);

BEGIN_EVENT_TABLE(PicViewCtrl, wxControl)
    EVT_PAINT(PicViewCtrl::OnPaint)
END_EVENT_TABLE()

void PicViewCtrl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    if(m_bClearFlag)
    {
        dc.Clear();
        int w, height;
        wxSize size = GetClientSize();
        wxString s;
        s.Printf(_T("No picture to show!"), size.x, size.y);
        dc.SetFont(*wxNORMAL_FONT);
        dc.GetTextExtent(s, &w, &height);
        height += 3;
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(0, 0, size.x, size.y);
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawLine(0, 0, size.x, size.y);
        dc.DrawLine(0, size.y, size.x, 0);
        dc.DrawText(s, (size.x-w)/2, ((size.y-(height))/2));
        return;
    }
    if(m_cViewBitmap.IsOk())
        dc.DrawBitmap(m_cViewBitmap, 0, 0, true);
}

void PicViewCtrl::SetBitmap(wxBitmap bitmap)
{
    m_bClearFlag = false;
    m_cViewBitmap = bitmap;
    m_CtrlSize.SetWidth(m_cViewBitmap.GetWidth());
    m_CtrlSize.SetHeight(m_cViewBitmap.GetHeight());
    this->SetSizeHints(m_CtrlSize);
    Refresh();
}

void PicViewCtrl::SetScale(const double dScale)
{
    m_dScaleRate = dScale;
}

void PicViewCtrl::SetLCUSize(const wxSize& size)
{
    m_LCUSize = size;
}

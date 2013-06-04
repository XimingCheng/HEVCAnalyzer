#include "PicViewCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(PicViewCtrl, wxControl);

BEGIN_EVENT_TABLE(PicViewCtrl, wxControl)
    EVT_PAINT(PicViewCtrl::OnPaint)
END_EVENT_TABLE()

void PicViewCtrl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    if(m_pViewBitmap)
        dc.DrawBitmap(*m_pViewBitmap, 0, 0, true);
}

void PicViewCtrl::SetBitmap(wxBitmap& bitmap)
{
    m_pViewBitmap = &bitmap;
    m_CtrlSize.SetWidth(m_pViewBitmap->GetWidth());
    m_CtrlSize.SetHeight(m_pViewBitmap->GetHeight());
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

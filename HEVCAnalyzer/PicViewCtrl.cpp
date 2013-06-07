#include "PicViewCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(PicViewCtrl, wxControl);

BEGIN_EVENT_TABLE(PicViewCtrl, wxControl)
    EVT_PAINT(PicViewCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(PicViewCtrl::OnEraseBkg)
    EVT_MOTION(PicViewCtrl::OnMouseMove)
END_EVENT_TABLE()

void PicViewCtrl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    Render(dc);
}

void PicViewCtrl::Render(wxDC& dc)
{
    if(!m_bClearFlag)
    {
        dc.SetBrush(wxBrush(wxColor(255,0,0,128)));
        dc.DrawRectangle(m_curLCUStart, wxSize(m_curLCUEnd.x - m_curLCUStart.x, m_curLCUEnd.y - m_curLCUStart.y));
    }
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

void PicViewCtrl::OnMouseMove(wxMouseEvent& event)
{
    if(!m_bClearFlag)
    {
        int id = GetCurLCURasterID(event.m_x, event.m_y);
        if(id != m_iLCURasterID)
        {
            m_iLCURasterID = id;
            if(id != -1)
                Refresh();
        }
    }
}

void PicViewCtrl::OnEraseBkg(wxEraseEvent& event)
{
    wxClientDC* clientDC = NULL;
    if (!event.GetDC())
        clientDC = new wxClientDC(this);
    wxDC* pDC = clientDC ? clientDC : event.GetDC();
    if(m_bClearFlag)
    {
        pDC->Clear();
        int w, height;
        wxSize size = GetClientSize();
        wxString s;
        s.Printf(_T("No picture to show!"), size.x, size.y);
        pDC->SetFont(*wxNORMAL_FONT);
        pDC->GetTextExtent(s, &w, &height);
        height += 3;
        pDC->SetBrush(*wxTRANSPARENT_BRUSH);
        pDC->SetPen(*wxLIGHT_GREY_PEN);
        pDC->DrawLine(0, 0, size.x, size.y);
        pDC->DrawLine(0, size.y, size.x, 0);
        pDC->DrawText(s, (size.x-w)/2, ((size.y-(height))/2));
        wxBitmap::CleanUpHandlers();
        return;
    }
    if(m_cViewBitmap.IsOk())
        pDC->DrawBitmap(m_cViewBitmap, 0, 0, true);
    if(clientDC)
        delete clientDC;
}

int PicViewCtrl::GetCurLCURasterID(long x, long y)
{
    int cuw = m_LCUSize.GetWidth();
    int cuh = m_LCUSize.GetHeight();
    int col_num = m_CtrlSize.GetWidth()/cuw + (m_CtrlSize.GetWidth()%cuw != 0);
    int row_num = m_CtrlSize.GetHeight()/cuh + (m_CtrlSize.GetHeight()%cuh != 0);
    int col = x/cuw;
    int row = y/cuh;
    if(col>= 0 && col < col_num && row >= 0 && row < row_num)
    {
        int x = (col+1)*cuw;
        int y = (row+1)*cuh;
        m_curLCUStart.x = col*cuw;
        m_curLCUStart.y = row*cuh;
        m_curLCUEnd.x   = (x > m_CtrlSize.GetWidth() ? m_CtrlSize.GetWidth() : x);
        m_curLCUEnd.y   = (y > m_CtrlSize.GetHeight() ? m_CtrlSize.GetHeight(): y);
        return (row*col_num + col);
    }
    return -1;
}

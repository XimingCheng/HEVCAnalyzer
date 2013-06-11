#include "PicViewCtrl.h"
#include <cmath>
#include <cassert>

IMPLEMENT_DYNAMIC_CLASS(PicViewCtrl, wxControl);

BEGIN_EVENT_TABLE(PicViewCtrl, wxControl)
    EVT_PAINT(PicViewCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(PicViewCtrl::OnEraseBkg)
    EVT_MOTION(PicViewCtrl::OnMouseMove)
    EVT_LEFT_DOWN(PicViewCtrl::OnMouseLButtonDown)
    EVT_LEFT_UP(PicViewCtrl::OnMouseLButtonUp)
    EVT_MOUSEWHEEL(PicViewCtrl::OnMouseWheel)
    EVT_KEY_DOWN(PicViewCtrl::OnKeyDown)
END_EVENT_TABLE()

void PicViewCtrl::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
    Render(gc);
}

void PicViewCtrl::Render(wxGraphicsContext* gc)
{
    if(!m_bClearFlag)
    {
        gc->Scale(m_dScaleRate, m_dScaleRate);
        DrawBackGround(gc);
        gc->SetBrush(wxBrush(wxColor(255, 0, 0, 128)));
        gc->DrawRectangle(m_curLCUStart.x, m_curLCUStart.y, m_curLCUEnd.x - m_curLCUStart.x, m_curLCUEnd.y - m_curLCUStart.y);
    }
    else
        DrawNoPictureTips(gc);
}

void PicViewCtrl::SetBitmap(wxBitmap bitmap)
{
    m_bClearFlag = false;
    m_cViewBitmap = bitmap;
    CalFitScaleRate();
    m_CtrlSize.SetWidth(m_dScaleRate*m_cViewBitmap.GetWidth());
    m_CtrlSize.SetHeight(m_dScaleRate*m_cViewBitmap.GetHeight());
    this->SetSizeHints(m_CtrlSize);
    GetParent()->FitInside();
    Refresh();
    g_LogMessage(wxString::Format(_T("SetBitmap m_dScaleRate %f"), m_dScaleRate));
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
        SetFocus();
        wxPoint pt = event.GetPosition();
        if(event.Dragging() && event.LeftIsDown())
        {
            int xper, yper;
            wxPoint pos = ClientToScreen(pt);
            wxScrolledWindow* pPar = (wxScrolledWindow*)GetParent();
            pPar->GetScrollPixelsPerUnit(&xper, &yper);
            pPar->Scroll((m_delta.x - pos.x)/xper, (m_delta.y - pos.y)/yper);
        }
        int id = GetCurLCURasterID(event.m_x/m_dScaleRate, event.m_y/m_dScaleRate);
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
}

int PicViewCtrl::GetCurLCURasterID(const double x, const double y)
{
    int cuw = m_LCUSize.GetWidth();
    int cuh = m_LCUSize.GetHeight();
    int col_num = m_cViewBitmap.GetWidth()/cuw + (m_cViewBitmap.GetWidth()%cuw != 0);
    int row_num = m_cViewBitmap.GetHeight()/cuh + (m_cViewBitmap.GetHeight()%cuh != 0);
    int col = x/cuw;
    int row = y/cuh;
    if(col >= 0 && col < col_num && row >= 0 && row < row_num)
    {
        int x = (col+1)*cuw;
        int y = (row+1)*cuh;
        m_curLCUStart.x = col*cuw;
        m_curLCUStart.y = row*cuh;
        m_curLCUEnd.x   = (x > m_cViewBitmap.GetWidth() ? m_cViewBitmap.GetWidth() : x);
        m_curLCUEnd.y   = (y > m_cViewBitmap.GetHeight() ? m_cViewBitmap.GetHeight(): y);
        return (row*col_num + col);
    }
    return -1;
}

void PicViewCtrl::OnMouseLButtonDown(wxMouseEvent& event)
{
    CaptureMouse();
    wxPoint pos = ClientToScreen(event.GetPosition());
    wxPoint origin = GetPosition();
    int dx = pos.x - origin.x;
    int dy = pos.y - origin.y;
    m_delta = wxPoint(dx, dy);
}

void PicViewCtrl::OnMouseLButtonUp(wxMouseEvent& event)
{
    if(HasCapture())
        ReleaseMouse();
}

void PicViewCtrl::OnMouseWheel(wxMouseEvent& event)
{
    if(m_bClearFlag)
    {
        event.Skip();
        return;
    }
    int direction = event.GetWheelRotation();
    int delta     = event.GetWheelDelta();
    if(event.CmdDown()) // scale
    {
        double bigger = (direction/delta)*m_dScaleRateStep;
        double rate   = ((m_dScaleRate + bigger) > m_dMaxScaleRate ? m_dMaxScaleRate : (m_dScaleRate + bigger));
        rate          = rate < m_dMinScaleRate ? m_dMinScaleRate : rate;
        if(fabs(rate - m_dScaleRate) > MINDIFF)
            m_bFitMode = false;
        ChangeScaleRate(rate);
        g_LogMessage(wxString::Format(_T("OnMouseWheel m_dScaleRate %f"), m_dScaleRate));
    }
    else
    {
        if(m_bMouseWheelPageUpDown)
        {
            int pages = -(direction/delta);
            ShowPageByDiffNumber(pages);
        }
        else
            event.Skip();
    }
}

void PicViewCtrl::CalMinMaxScaleRate()
{
    if(!m_bClearFlag && !m_cViewBitmap.IsNull());
    {
        if(!m_cViewBitmap.IsOk())
            return;
        int width    = m_cViewBitmap.GetWidth();
        int height   = m_cViewBitmap.GetHeight();
        // get the screen size to cal min max scale rate
        int scrwidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
        int scrheight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
        double sratew = scrwidth/static_cast<double>(width);
        double srateh = scrheight/static_cast<double>(height);
        double srate  = sratew < srateh ? sratew : srateh;
        m_dMaxScaleRate  = 1.5*srate < 2.0 ? 2.0 : 1.5*srate;
        m_dMinScaleRate  = 50/static_cast<double>(width);
        if(m_dMinScaleRate > m_dMaxScaleRate)
        {
            // this situation may be terrible, here just exchange each other
            double tmp      = m_dMinScaleRate;
            m_dMinScaleRate = m_dMaxScaleRate;
            m_dMaxScaleRate = tmp;
        }
        m_dScaleRateStep = (m_dMaxScaleRate - m_dMinScaleRate)/100.0;
    }
}

void PicViewCtrl::CalFitScaleRate()
{
    if(!m_bClearFlag && !m_cViewBitmap.IsNull());
    {
        if(!m_cViewBitmap.IsOk())
            return;
        // get the FitScaleRate, which is the smaller one frome ratew and rateh
        int width    = m_cViewBitmap.GetWidth();
        int height   = m_cViewBitmap.GetHeight();
        int cwidth   = GetParent()->GetSize().GetWidth();
        int cheight  = GetParent()->GetSize().GetHeight();
        double ratew = cwidth/static_cast<double>(width);
        double rateh = cheight/static_cast<double>(height);
        m_dFitScaleRate = ratew < rateh ? ratew : rateh;
        if(m_bFitMode)
            ChangeScaleRate(m_dFitScaleRate);
    }
}

void PicViewCtrl::ChangeScaleRate(const double rate)
{
    if(fabs(rate - m_dScaleRate) > MINDIFF)
    {
        m_dScaleRate = rate;
        m_CtrlSize.SetWidth(m_dScaleRate*m_cViewBitmap.GetWidth());
        m_CtrlSize.SetHeight(m_dScaleRate*m_cViewBitmap.GetHeight());
        this->SetSizeHints(m_CtrlSize);
        GetParent()->FitInside();
        Refresh();
    }
}

// if diff > 0 show next, else show previous
bool PicViewCtrl::ShowPageByDiffNumber(const int diff)
{
    int cnt     = m_pList->GetItemCount();
    int cursel  = m_pList->GetSelection();
    int nextsel = 0;
    if(cursel != wxNOT_FOUND)
    {
        nextsel = ((cursel+diff) > cnt-1 ? cnt-1 : (cursel+diff));
        nextsel = nextsel < 0 ? 0 : nextsel;
    }
    if(nextsel != cursel)
    {
        m_pList->SetSelection(nextsel);
        wxCommandEvent evt(wxEVT_COMMAND_LISTBOX_SELECTED, wxID_ANY);
        evt.SetInt(nextsel);
        wxPostEvent(m_pFrame, evt);
        return true;
    }
    return false;
}

void PicViewCtrl::MoveLCURect(const Direction& d)
{
    if(m_iLCURasterID == -1)
        return;
    int cuw = m_LCUSize.GetWidth();
    int cuh = m_LCUSize.GetHeight();
    int col_num = m_cViewBitmap.GetWidth()/cuw + (m_cViewBitmap.GetWidth()%cuw != 0);
    int row_num = m_cViewBitmap.GetHeight()/cuh + (m_cViewBitmap.GetHeight()%cuh != 0);
    int maxid   = row_num*col_num - 1;
    int id = -1;
    switch(d)
    {
    case MOVE_UP:
        id = m_iLCURasterID - col_num;
        break;
    case MOVE_DOWN:
        id = m_iLCURasterID + col_num;
        break;
    case MOVE_LEFT:
        id = m_iLCURasterID - 1;
        break;
    case MOVE_RIGHT:
        id = m_iLCURasterID + 1;
        break;
    default:
        assert(0);
        break;
    }
    if(id == -1 && d == MOVE_LEFT) id = 0;
    else if(id == maxid + 1 && d == MOVE_RIGHT) id = maxid;
    else
    {
        id = id > maxid ? id - col_num : id;
        id = id < 0 ? id + col_num : id;
    }
    if(id != m_iLCURasterID)
    {
        CalStartEndPointByLCUId(id);
        m_iLCURasterID = id;
        Refresh();
    }
}

void PicViewCtrl::OnKeyDown(wxKeyEvent& event)
{
    if(m_bClearFlag)
        return;
    int key = event.GetKeyCode();
    switch(key)
    {
    case WXK_PAGEUP:
        ShowPageByDiffNumber(-1);
        break;
    case WXK_PAGEDOWN:
        ShowPageByDiffNumber(1);
        break;
    case WXK_UP:
        MoveLCURect(MOVE_UP);
        break;
    case WXK_DOWN:
        MoveLCURect(MOVE_DOWN);
        break;
    case WXK_LEFT:
        MoveLCURect(MOVE_LEFT);
        break;
    case WXK_RIGHT:
        MoveLCURect(MOVE_RIGHT);
        break;
    default:
        event.Skip();
        break;
    }
}

void PicViewCtrl::CalStartEndPointByLCUId(const int id)
{
    int cuw = m_LCUSize.GetWidth();
    int cuh = m_LCUSize.GetHeight();
    int col_num = m_cViewBitmap.GetWidth()/cuw + (m_cViewBitmap.GetWidth()%cuw != 0);
    int row_num = m_cViewBitmap.GetHeight()/cuh + (m_cViewBitmap.GetHeight()%cuh != 0);
    int col = id%col_num;
    int row = id/col_num;
    if(col >= 0 && col < col_num && row >= 0 && row < row_num)
    {
        int x = (col+1)*cuw;
        int y = (row+1)*cuh;
        m_curLCUStart.x = col*cuw;
        m_curLCUStart.y = row*cuh;
        m_curLCUEnd.x   = (x > m_cViewBitmap.GetWidth() ? m_cViewBitmap.GetWidth() : x);
        m_curLCUEnd.y   = (y > m_cViewBitmap.GetHeight() ? m_cViewBitmap.GetHeight() : y);
    }
}

void PicViewCtrl::DrawGrid(wxGraphicsContext* gc)
{
    int cuw = m_LCUSize.GetWidth();
    int cuh = m_LCUSize.GetHeight();
    int col_num = m_cViewBitmap.GetWidth()/cuw + (m_cViewBitmap.GetWidth()%cuw != 0);
    int row_num = m_cViewBitmap.GetHeight()/cuh + (m_cViewBitmap.GetHeight()%cuh != 0);
    gc->SetPen(*wxBLACK_PEN);
    for(int i = 1; i < row_num; i++)
        gc->StrokeLine(0, i*cuh, m_cViewBitmap.GetWidth(), i*cuh);
    for(int i = 1; i < col_num; i++)
        gc->StrokeLine(i*cuw, 0, i*cuw, m_cViewBitmap.GetHeight());
}

void PicViewCtrl::DrawBackGround(wxGraphicsContext* gc)
{
    gc->DrawBitmap(m_cViewBitmap, 0, 0, m_cViewBitmap.GetWidth(), m_cViewBitmap.GetHeight());
    if(m_bShowPUType)
    {
        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->SetBrush(wxBrush(wxColor(255, 0, 0, 50)));
        gc->DrawRectangle(0, 0, m_cViewBitmap.GetWidth(), m_cViewBitmap.GetHeight());
    }
    if(m_bShowGrid)
        DrawGrid(gc);
}

void PicViewCtrl::DrawNoPictureTips(wxGraphicsContext* gc)
{
    double w, h, descent, externalLeading;
    wxSize size = GetClientSize();
    wxString s;
    s.Printf(_T("No picture to show!"), size.x, size.y);
    gc->SetFont(*wxNORMAL_FONT, *wxBLACK);
    gc->SetPen(*wxBLACK_PEN);
    gc->GetTextExtent(s, &w, &h, &descent, &externalLeading);
    gc->SetBrush(wxBrush(wxColor(255, 255, 255, 255)));
    gc->DrawRectangle(0, 0, size.GetWidth()-1, size.GetHeight()-1);
    h += 3;
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    gc->SetPen(*wxLIGHT_GREY_PEN);
    gc->StrokeLine(0, 0, size.x, size.y);
    gc->StrokeLine(0, size.y, size.x, 0);
    gc->DrawText(s, (size.x-w)/2, ((size.y-(h))/2));
    wxBitmap::CleanUpHandlers();
}

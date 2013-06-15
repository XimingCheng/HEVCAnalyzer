#include "PixelViewCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(PixelViewCtrl, wxScrolledWindow);

BEGIN_EVENT_TABLE(PixelViewCtrl, wxScrolledWindow)
    EVT_ERASE_BACKGROUND(PixelViewCtrl::OnEraseBkg)
    EVT_MIDDLE_UP(PixelViewCtrl::OnMouseMidUp)
    EVT_LEFT_DOWN(PixelViewCtrl::OnLeftButtonDown)
    EVT_LEFT_UP(PixelViewCtrl::OnLeftButtonUp)
    EVT_MOTION(PixelViewCtrl::OnMouseMove)
    EVT_TIMER(TIMER_ID, PixelViewCtrl::OnTimer)
    EVT_LEAVE_WINDOW(PixelViewCtrl::OnLeaveWindow)
    EVT_ENTER_WINDOW(PixelViewCtrl::OnEnterWindow)
END_EVENT_TABLE()

void PixelViewCtrl::OnDraw(wxDC& dc)
{
    //wxAutoBufferedPaintDC tmpdc(this);
    int xbase, ybase;
    CalcUnscrolledPosition(0, 0, &xbase, &ybase);
//    g_LogMessage(wxString::Format(_T("base x:%d,y:%d"), xbase, xbase));
    int virtualwidth, virtualheight;
    GetClientSize(&virtualwidth, &virtualheight);
//    g_LogMessage(wxString::Format(_T("size x:%d y:%d"), virtualwidth, virtualheight));

    DrawBackground(dc, xbase, ybase, xbase+virtualwidth, ybase+virtualheight);
    DrawGrid(dc, xbase, ybase, xbase+virtualwidth, ybase+virtualheight);
    DrawFocusLine(dc);
    int xindexstart = max(0, (xbase-m_iXOffset)/m_iWidthPerPixel-1);
    int xindexend = min(m_iCUWidth-1, (xbase+virtualwidth-m_iXOffset)/m_iWidthPerPixel+1);
    int yindexstart = max(0, (ybase-m_iYOffset)/m_iHeightPerPixel-1);
    int yindexend = min(m_iCUHeight-1, (ybase+virtualheight-m_iYOffset)/m_iHeightPerPixel+1);
//    g_LogMessage(wxString::Format(_T("x %d-%d"), xindexstart, xindexend));
//    g_LogMessage(wxString::Format(_T("y %d-%d"), yindexstart, yindexend));
    for(int i = xindexstart; i <= xindexend; i++)
        for(int j = yindexstart; j <= yindexend; j++)
            ShowOneCell(dc, i, j, i, 100, 100);
//    dc.DrawLine(0, 0, 100, 100);
}

void PixelViewCtrl::OnEraseBkg(wxEraseEvent& event)
{
}

void PixelViewCtrl::DrawGrid(wxDC& dc, int xstart, int ystart, int xend, int yend)
{
    wxPen oldpen = dc.GetPen();
    dc.SetPen(*wxBLACK_PEN);
    xstart = max(xstart, m_iXOffset);
    ystart = max(ystart, m_iYOffset);
    xend = min(xend, m_iXOffset+m_iCUWidth*m_iWidthPerPixel);
    yend = min(yend, m_iYOffset+m_iCUHeight*m_iHeightPerPixel);
    /* draw the horizontal line*/
    int horstart = ((ystart-m_iYOffset)/m_iHeightPerPixel-1)*m_iHeightPerPixel+m_iYOffset;
    horstart = max(horstart, m_iYOffset);
    int horend = ((yend-m_iYOffset)/m_iHeightPerPixel+1)*m_iHeightPerPixel+m_iYOffset;
    horend = min(horend, m_iYOffset+m_iHeightPerPixel*m_iHeightPerPixel);
    for(int i = horstart; i <= horend; i+=m_iHeightPerPixel)
        dc.DrawLine(xstart, i, xend, i);
    /* draw the vertical line */
    int verstart = ((xstart-m_iXOffset)/m_iWidthPerPixel-1)*m_iWidthPerPixel+m_iXOffset;
    verstart = max(verstart, m_iXOffset);
    int verend = ((xend-m_iXOffset)/m_iWidthPerPixel+1)*m_iWidthPerPixel+m_iXOffset;
    verend = min(verend, m_iXOffset+m_iCUWidth*m_iWidthPerPixel);
    for(int i = verstart; i <= verend; i+=m_iWidthPerPixel)
        dc.DrawLine(i, ystart, i, yend);
    dc.SetPen(oldpen);
}

void PixelViewCtrl::DrawBackground(wxDC& dc, int xstart, int ystart,
                                   int xend, int yend)
{
    wxPen oldpen = dc.GetPen();
    wxBrush oldBrush = dc.GetBrush();
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(xstart, ystart, xend, yend);

    dc.SetPen(oldpen);
    dc.SetBrush(oldBrush);
}

void PixelViewCtrl::DrawFocusLine(wxDC& dc)
{
    wxPen oldpen = dc.GetPen();
    wxPen newpen(*wxRED, 2);
    dc.SetPen(newpen);

    int xpos = m_FocusPos.x;
    int ypos = m_FocusPos.y;
    dc.DrawLine(xpos*m_iWidthPerPixel+m_iXOffset, (ypos+1)*m_iHeightPerPixel+m_iYOffset,
                xpos*m_iWidthPerPixel+m_iXOffset, m_iYOffset);
    dc.DrawLine((xpos+1)*m_iWidthPerPixel+m_iXOffset, ypos*m_iHeightPerPixel+m_iYOffset,
                m_iXOffset, ypos*m_iHeightPerPixel+m_iYOffset);
    dc.DrawLine((xpos+1)*m_iWidthPerPixel+m_iXOffset, (ypos+1)*m_iHeightPerPixel+m_iYOffset,
                (xpos+1)*m_iWidthPerPixel+m_iXOffset, ypos*m_iHeightPerPixel+m_iYOffset);
    dc.DrawLine((xpos+1)*m_iWidthPerPixel+m_iXOffset, (ypos+1)*m_iHeightPerPixel+m_iYOffset,
                 xpos*m_iWidthPerPixel+m_iXOffset, (ypos+1)*m_iHeightPerPixel+m_iYOffset);

    dc.SetPen(oldpen);
}

void PixelViewCtrl::OnMouseMidUp(wxMouseEvent& event)
{
    m_bScrollMode = !m_bScrollMode;
    if(true == m_bScrollMode)
    {
        SetCursor(wxCursor(wxCURSOR_CROSS));
    }
    else
    {
        if(m_pTimer->IsRunning())
            m_pTimer->Stop();
        SetCursor(wxNullCursor);
    }
}

void PixelViewCtrl::OnMouseMove(wxMouseEvent & event)
{
    if (false == m_bScrollMode)
        return;
    wxPoint pt = event.GetPosition();
    int xLogic, yLogic;
    CalcUnscrolledPosition(pt.x, pt.y, &xLogic, &yLogic);
    int xpixels, ypixels;
    GetScrollPixelsPerUnit(&xpixels, &ypixels);
    m_iXCurrentUnit = (xLogic-pt.x)/xpixels;
    m_iYCurrentUnit = (yLogic-pt.y)/ypixels;
    wxSize size = GetClientSize();
    if(pt.x < 0.15*size.GetWidth())
        m_iXStep = -3;
    else if(pt.x > 0.85*size.GetWidth())
        m_iXStep = 3;
    else
        m_iXStep = 0;
    if(pt.y < 0.15*size.GetHeight())
        m_iYStep = -3;
    else if(pt.y > 0.85*size.GetHeight())
        m_iYStep = 3;
    else
        m_iYStep = 0;

    if(m_pTimer->IsRunning())
        return;
    else
    {
        g_LogMessage(_T(" start timer"));
        m_pTimer->Start(200);
    }

//    g_LogMessage(wxString::Format(_T("now logic pos: %d\t %d"), xLogic, yLogic));
//    g_LogMessage(wxString::Format(_T("now size: %d\t %d"), size.GetWidth(), size.GetHeight()));
//    g_LogMessage(wxString::Format(_T("now pos: %d\t %d"), pt.x, pt.y));
}
void PixelViewCtrl::OnTimer(wxTimerEvent& event)
{
    m_iXCurrentUnit += m_iXStep;
    m_iYCurrentUnit += m_iYStep;
    Scroll(m_iXCurrentUnit, m_iYCurrentUnit);
}
    
void PixelViewCtrl::OnLeaveWindow(wxMouseEvent& event)
{
    if(m_bScrollMode)
    {
        m_pTimer->Stop();
    }
    g_LogMessage(_T("Leave Window"));
}

void PixelViewCtrl::OnEnterWindow(wxMouseEvent& event)
{
    if(m_bScrollMode)
    {
        SetCursor(wxCursor(wxCURSOR_CROSS));
    }
    g_LogMessage(_T("Enter Window"));
}
void PixelViewCtrl::ShowOneCell(wxDC& dc, const int xIndex, const int yIndex,
                         const int y, const int u, const int v)
{
    wxString text;
    if(m_bHexFormat)
        text = wxString::Format(_T("0X%X\n0X%X\n0X%X\n(%d,%d)"), y, u, v, xIndex, yIndex);
    else
        text = wxString::Format(_T("%d\n%d\n%d\n(%d, %d)"), y, u, v, xIndex, yIndex);
    dc.DrawText(text, m_iXOffset+xIndex*m_iWidthPerPixel, m_iYOffset+yIndex*m_iHeightPerPixel);
}

void PixelViewCtrl::LogicPosToIndex(int xLogic, int yLogic, int *xIndex, int *yIndex)
{
    *xIndex = (xLogic-m_iXOffset)/m_iWidthPerPixel;
    *yIndex = (yLogic-m_iYOffset)/m_iHeightPerPixel;
}
void PixelViewCtrl:: OnLeftButtonDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    int xlogic, ylogic;
    CalcUnscrolledPosition(pos.x, pos.y, &xlogic, &ylogic);
    LogicPosToIndex(xlogic, ylogic, &(m_LeftDownPos.x), &(m_LeftDownPos.y));

    g_LogMessage(wxString::Format(_T("%d %d"), m_LeftDownPos.x, m_LeftDownPos.y));
}
void PixelViewCtrl:: OnLeftButtonUp(wxMouseEvent& event)
{
    wxPoint newpos = event.GetPosition();
    int xlogic, ylogic;
    CalcUnscrolledPosition(newpos.x, newpos.y, &xlogic, &ylogic);
    LogicPosToIndex(xlogic, ylogic, &(newpos.x), &(newpos.y));
    if(m_LeftDownPos.x != newpos.x || m_LeftDownPos.y != newpos.y)
        return;
    SetFocusPos(newpos);
    Refresh();
}
void PixelViewCtrl::SetFocusPos(const wxPoint& pos)
{
    m_FocusPos.x = pos.x;
    m_FocusPos.y = pos.y;
}













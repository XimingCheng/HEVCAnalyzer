#include "PixelViewCtrl.h"

DEFINE_EVENT_TYPE(wxEVT_YUVBUFFER_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_POSITION_CHANGED)

IMPLEMENT_DYNAMIC_CLASS(PixelViewCtrl, wxScrolledWindow);

BEGIN_EVENT_TABLE(PixelViewCtrl, wxScrolledWindow)
    EVT_PAINT(PixelViewCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(PixelViewCtrl::OnEraseBkg)
    EVT_MIDDLE_UP(PixelViewCtrl::OnMouseMidUp)
    EVT_LEFT_DOWN(PixelViewCtrl::OnLeftButtonDown)
    EVT_LEFT_UP(PixelViewCtrl::OnLeftButtonUp)
    EVT_MOTION(PixelViewCtrl::OnMouseMove)
    EVT_TIMER(TIMER_ID, PixelViewCtrl::OnTimer)
    EVT_LEAVE_WINDOW(PixelViewCtrl::OnLeaveWindow)
    EVT_ENTER_WINDOW(PixelViewCtrl::OnEnterWindow)
    EVT_COMMAND(wxID_ANY, wxEVT_YUVBUFFER_CHANGED, PixelViewCtrl::OnBufferChanged)
    EVT_COMMAND(wxID_ANY, wxEVT_POSITION_CHANGED, PixelViewCtrl::OnPosChanged)
END_EVENT_TABLE()

void PixelViewCtrl::OnPaint(wxPaintEvent& event)
{
    //wxAutoBufferedPaintDC dc(this);
    wxPaintDC dc(this);
    DoPrepareDC(dc);
    Render(dc);
}

void PixelViewCtrl::Render(wxDC &dc)
{
    if(!m_pBuffer)
    {
        wxString str = _T("No Pixel Value");
        wxFont oldfont = dc.GetFont();
        wxColour oldcolor = dc.GetTextForeground();
        dc.SetFont(*wxSMALL_FONT);
        dc.SetTextForeground(wxColour(55, 86, 132));
        int textwidth, textheight;
        dc.GetTextExtent(str, &textwidth, &textheight);
        int clientwidth, clientheight;
        GetClientSize(&clientwidth, &clientheight);

        DrawBackground(dc, 0, 0, clientwidth, clientheight);

        SetVirtualSize(clientwidth, clientheight);
        dc.DrawText(str, (clientwidth-textwidth)/2, (clientheight-textheight)/2);

        dc.SetFont(oldfont);
        dc.SetTextForeground(oldcolor);

    }
    else
    {
        AdaptiveSize(dc);
        SetVirtualSize(2*m_iXOffset+m_iCUWidth*m_iWidthPerPixel,
                       2*m_iYOffset+m_iCUHeight*m_iHeightPerPixel);
        SetScrollRate(m_iWidthPerPixel/5, m_iHeightPerPixel/5);
        int virtualwidth, virtualheight;
        GetClientSize(&virtualwidth, &virtualheight);
        int xbase, ybase;
        if(m_bPaintEventSource)
        {
            CalcUnscrolledPosition(0, 0, &xbase, &ybase);
            m_bPaintEventSource = false;
        }
        else
        {
            CalcUnscrolledPosition(0, 0, &xbase, &ybase);
        }
        SetRulerCtrlFited();
        int xindexstart = max(0, (xbase-m_iXOffset)/m_iWidthPerPixel-1);
        int xindexend = min(m_iCUWidth-1, (xbase+virtualwidth-m_iXOffset)/m_iWidthPerPixel+1);
        int yindexstart = max(0, (ybase-m_iYOffset)/m_iHeightPerPixel-1);
        int yindexend = min(m_iCUHeight-1, (ybase+virtualheight-m_iYOffset)/m_iHeightPerPixel+1);
//        g_LogMessage(wxString::Format(_T("x %d-%d"), xindexstart, xindexend));
//        g_LogMessage(wxString::Format(_T("y %d-%d"), yindexstart, yindexend));

        if(IsDoubleBuffered())
        {
            DrawBackground(dc, xbase, ybase, xbase+virtualwidth, ybase+virtualheight);
            DrawGrid(dc, xbase, ybase, xbase+virtualwidth, ybase+virtualheight);
            DrawFocusLine(dc);
            for(int i = xindexstart; i <= xindexend; i++)
                for(int j = yindexstart; j <= yindexend; j++)
                    ShowOneCell(dc, i, j);
        }
        else
        {
            wxMemoryDC mdc;
            wxBitmap bm = wxBitmap(2*m_iXOffset+m_iCUWidth*m_iWidthPerPixel+30, 2*m_iYOffset+m_iCUHeight*m_iHeightPerPixel+30);
            mdc.SelectObject(bm);
            DrawBackground(mdc, xbase, ybase, xbase+virtualwidth, ybase+virtualheight);
            DrawGrid(mdc, xbase, ybase, xbase+virtualwidth, ybase+virtualheight);
            DrawFocusLine(mdc);
            for(int i = xindexstart; i <= xindexend; i++)
                for(int j = yindexstart; j <= yindexend; j++)
                    ShowOneCell(mdc, i, j);
            dc.Blit(xbase, ybase, virtualwidth, virtualheight, &mdc, xbase, ybase);
        }

    }
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
    horend = min(horend, m_iYOffset+m_iCUHeight*m_iHeightPerPixel);
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
    dc.DrawLine(xpos*m_iWidthPerPixel+m_iXOffset, (ypos+1)*m_iHeightPerPixel+m_iYOffset, xpos*m_iWidthPerPixel+m_iXOffset, m_iYOffset);
    dc.DrawLine((xpos+1)*m_iWidthPerPixel+m_iXOffset, ypos*m_iHeightPerPixel+m_iYOffset, m_iXOffset, ypos*m_iHeightPerPixel+m_iYOffset);
    dc.DrawLine((xpos+1)*m_iWidthPerPixel+m_iXOffset, (ypos+1)*m_iHeightPerPixel+m_iYOffset, (xpos+1)*m_iWidthPerPixel+m_iXOffset, ypos*m_iHeightPerPixel+m_iYOffset);
    dc.DrawLine((xpos+1)*m_iWidthPerPixel+m_iXOffset, (ypos+1)*m_iHeightPerPixel+m_iYOffset, xpos*m_iWidthPerPixel+m_iXOffset, (ypos+1)*m_iHeightPerPixel+m_iYOffset);

    dc.SetPen(oldpen);
}

void PixelViewCtrl::OnMouseMidUp(wxMouseEvent& event)
{
    m_bScrollMode = !m_bScrollMode;
    if(m_bScrollMode)
    {
        SetCursor(wxCursor(wxCURSOR_SIZING));
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
    if (!m_bScrollMode)
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
    SetFocus();
    if(m_bScrollMode)
    {
        SetCursor(wxCursor(wxCURSOR_SIZING));
    }
    g_LogMessage(_T("Enter Window"));
}

void PixelViewCtrl::ShowOneCell(wxDC& dc, const int xIndex, const int yIndex)
{
    int i, j;
    if(!m_pBlockInfo)
    {
        i = 0;
        j = 0;
    }
    else
    {
        j = yIndex + m_pBlockInfo->_iBlockY;
        i = xIndex + m_pBlockInfo->_iBlockX;
    }
    Pel* pY = m_pBuffer->getLumaAddr() + j*m_pBuffer->getStride();
    Pel* pU = m_pBuffer->getCbAddr()   + (j/2)*m_pBuffer->getCStride();
    Pel* pV = m_pBuffer->getCrAddr()   + (j/2)*m_pBuffer->getCStride();
    int  y = pY[i];
    int  u = pU[i/2];
    int  v = pV[i/2];

    wxString Ystr, Ustr, Vstr, Posstr;
    Posstr = wxString::Format(_T("(%d,%d)"), xIndex, yIndex);
    if(m_bHexFormat)
    {
        Ystr = wxString::Format(_T("%04x"), y);
        Ustr = wxString::Format(_T("%04x"), u);
        Vstr = wxString::Format(_T("%04x"), v);
    }
    else
    {
        Ystr = wxString::Format(_T("%04d"), y);
        Ustr = wxString::Format(_T("%04d"), u);
        Vstr = wxString::Format(_T("%04d"), v);
    }

    int gap = 2;
    int textheight, textwidth;
    int xbase = m_iXOffset+xIndex*m_iWidthPerPixel;
    int ybase = m_iYOffset+yIndex*m_iHeightPerPixel;

    wxFont oldfont = dc.GetFont();
    wxColour oldcolor = dc.GetTextForeground();
    dc.SetFont(*wxSMALL_FONT);
    dc.SetTextForeground(wxColour(55, 86, 132));
    dc.GetTextExtent(Ystr, &textwidth, &textheight);
    int ystart = (m_iHeightPerPixel-4*textheight-3*gap)/2;
    int xstart = (m_iWidthPerPixel-textwidth)/2;
    dc.DrawText(Ystr, xbase+xstart, ybase+ystart);

    dc.GetTextExtent(Ustr, &textwidth, &textheight);
    xstart = (m_iWidthPerPixel-textwidth)/2;
    dc.DrawText(Ustr, xbase+xstart, ybase+ystart+gap+textheight);

    dc.GetTextExtent(Vstr, &textwidth, &textheight);
    xstart = (m_iWidthPerPixel-textwidth)/2;
    dc.DrawText(Vstr, xbase+xstart, ybase+ystart+2*(gap+textheight));

    wxString posstr = wxString::Format(_T("(%d,%d)"), xIndex, yIndex);
    dc.GetTextExtent(posstr, &textwidth, &textheight);
    xstart = (m_iWidthPerPixel-textwidth)/2;
    dc.DrawText(posstr, xbase+xstart, ybase+ystart+3*(gap+textheight));


    dc.SetFont(oldfont);
    dc.SetTextForeground(oldcolor);
}

void PixelViewCtrl::LogicPosToIndex(int xLogic, int yLogic, int *xIndex, int *yIndex)
{
    *xIndex = (xLogic-m_iXOffset)/m_iWidthPerPixel;
    *yIndex = (yLogic-m_iYOffset)/m_iHeightPerPixel;
}

void PixelViewCtrl:: OnLeftButtonDown(wxMouseEvent& event)
{
    if(!m_pBuffer) return;
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
    if(newpos.x >= m_iCUWidth || newpos.y >=m_iCUHeight)
        return;
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

void PixelViewCtrl::AdaptiveSize(wxDC& dc)
{
    wxFont oldfont = dc.GetFont();
    dc.SetFont(*wxSMALL_FONT);
    int textwidth, textheight;
    dc.GetTextExtent(_T("ffff"),&textwidth, &textheight);
    int gap = 1;
    m_iHeightPerPixel = 4*textheight+3*gap+10;
    m_iWidthPerPixel = textwidth+40;

    dc.SetFont(oldfont);
}

void PixelViewCtrl::OnBufferChanged(wxCommandEvent& event)
{
    m_pBuffer = (TComPicYuv*)event.GetClientData();
    m_FocusPos.x = 0;
    m_FocusPos.y = 0;
    Scroll(0, 0);
    Refresh();
}

void PixelViewCtrl::OnPosChanged(wxCommandEvent& event)
{
    m_pBlockInfo = (MSG_block_pos*)event.GetClientData();
    SetCUHeight(m_pBlockInfo->_iBlockHeight);
    SetCUWidth(m_pBlockInfo->_iBlockWidth);
    m_FocusPos.x = m_pBlockInfo->_iOffsetX;
    m_FocusPos.y = m_pBlockInfo->_iOffsetY;
    m_bPaintEventSource = true;
    wxClientDC dc(this);
    AdaptiveSize(dc);
    SetVirtualSize(2*m_iXOffset+m_iCUWidth*m_iWidthPerPixel,
                   2*m_iYOffset+m_iCUHeight*m_iHeightPerPixel);
    SetScrollRate(m_iWidthPerPixel/5, m_iHeightPerPixel/5);
    int virtualwidth, virtualheight;
    GetClientSize(&virtualwidth, &virtualheight);
    int xbase, ybase;
    xbase = max(0, m_FocusPos.x*m_iWidthPerPixel+m_iXOffset-(virtualwidth-m_iWidthPerPixel)/2);
    xbase = xbase > 2*m_iXOffset+m_iWidthPerPixel*m_iCUWidth-virtualwidth ?
            2*m_iXOffset+m_iWidthPerPixel*m_iCUWidth-virtualwidth : xbase;
    ybase = max(0, m_FocusPos.y*m_iHeightPerPixel+m_iYOffset-(virtualheight-m_iHeightPerPixel)/2);
    ybase = ybase > 2*m_iYOffset+m_iHeightPerPixel*m_iCUHeight-virtualheight ?
            2*m_iYOffset+m_iHeightPerPixel*m_iCUHeight-virtualheight : ybase;
    Scroll(xbase/(m_iWidthPerPixel/5), ybase/(m_iHeightPerPixel/5));
    Refresh();
}

void PixelViewCtrl::Clear()
{
    m_pTimer->Stop();
    m_pBuffer = NULL;
    m_pBlockInfo = NULL;
    m_iCUHeight = 32;
    m_iCUWidth = 32;
    m_FocusPos.x = 0;
    m_FocusPos.y = 0;
    m_iHeightPerPixel = 90;
    m_iWidthPerPixel = 70;
    m_bHexFormat = true;
    Refresh();
}

void PixelViewCtrl::SetRulerCtrlFited()
{
    int xbase, ybase;
    CalcUnscrolledPosition(0, 0, &xbase, &ybase);
    m_pHRuler->SetStartValue(0);
    m_pVRuler->SetStartValue(0);
    m_pHRuler->SetStartPos(-xbase + m_pHRuler->GetRulerWidth() + m_iXOffset);
    m_pVRuler->SetStartPos(-ybase + m_iYOffset);
    m_pHRuler->SetEndValue(m_iCUWidth);
    m_pVRuler->SetEndValue(m_iCUHeight);
    m_pHRuler->SetValuePerUnit(1);
    m_pVRuler->SetValuePerUnit(1);
    m_pHRuler->SetLongMarkLen(8);
    m_pVRuler->SetLongMarkLen(8);
    m_pHRuler->SetTextMarkLen(1);
    m_pVRuler->SetTextMarkLen(1);
    m_pHRuler->SetScaleRate(m_iWidthPerPixel);
    m_pVRuler->SetScaleRate(m_iHeightPerPixel);
}

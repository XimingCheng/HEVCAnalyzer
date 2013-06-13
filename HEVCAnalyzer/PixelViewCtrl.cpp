#include "PixelViewCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(PixelViewCtrl, wxScrolledWindow);

BEGIN_EVENT_TABLE(PixelViewCtrl, wxScrolledWindow)
END_EVENT_TABLE()

void PixelViewCtrl::OnDraw(wxDC& dc)
{
    //wxAutoBufferedPaintDC tmpdc(this);
    DrawGrid(dc);
    DrawFocusLine(dc);
}

void PixelViewCtrl::OnEraseBkg(wxEraseEvent& event)
{
}

void PixelViewCtrl::DrawGrid(wxDC& dc)
{
    int XOffset = 20;
    int YOffset = 20;
    wxPen oldpen = dc.GetPen();
    dc.SetPen(*wxBLACK_PEN);
    for(int i = 0; i <= m_iCUHeight; i++)
        dc.DrawLine(XOffset, YOffset+i*m_iHeightPerPixel, 
                    XOffset+m_iCUWidth*m_iWidthPerPixel, YOffset+i*m_iHeightPerPixel);
    for(int i = 0; i <= m_iCUWidth; i++)
        dc.DrawLine(XOffset+i*m_iWidthPerPixel, YOffset, 
                    XOffset+i*m_iWidthPerPixel, YOffset+m_iCUHeight*m_iHeightPerPixel);
    dc.SetPen(oldpen);
}

void PixelViewCtrl::DrawFocusLine(wxDC& dc)
{
    int XOffset = 20;
    int YOffset = 20;

    wxPen oldpen = dc.GetPen();
    wxPen newpen(*wxRED, 2);
    dc.SetPen(newpen);

    int xpos = m_FocusPos.x;
    int ypos = m_FocusPos.y;
    dc.DrawLine(xpos*m_iWidthPerPixel+XOffset, ypos*m_iHeightPerPixel+YOffset,
                xpos*m_iWidthPerPixel+XOffset, YOffset);
    dc.DrawLine(xpos*m_iWidthPerPixel+XOffset, ypos*m_iHeightPerPixel+YOffset,
                XOffset, ypos*m_iHeightPerPixel+YOffset);

    dc.SetPen(oldpen);
}

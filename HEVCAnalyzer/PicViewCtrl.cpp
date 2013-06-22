#include "PicViewCtrl.h"

extern const wxEventType wxEVT_YUVBUFFER_CHANGED;
extern const wxEventType wxEVT_POSITION_CHANGED;

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

PicViewCtrl::PicViewCtrl(wxWindow* parent, wxWindowID id, wxSimpleHtmlListBox* pList, RulerCtrl* pHRuler, RulerCtrl* pVRuler,
            wxWindow* pPixelCtrl, wxFrame* pFrame)
    : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxWANTS_CHARS),
    m_bClearFlag(true), m_bFitMode(true), m_dScaleRate(1.0), m_dMinScaleRate(0.1), m_dMaxScaleRate(2.0), m_dFitScaleRate(1.0),
    m_dScaleRateStep(0.02), m_delta(-1, -1), m_curLCUStart(-1, -1), m_curLCUEnd(-1, -1), m_iLCURasterID(-1), m_pList(pList),
    m_pFrame(pFrame), m_bShowGrid(true), m_bMouseWheelPageUpDown(false), m_bShowPUType(true), m_pBuffer(NULL),
    m_iYUVBit(8), m_iShowWhich_O_Y_U_V(MODE_ORG), m_pHRuler(pHRuler), m_pVRuler(pVRuler), m_bFullRefresh(true),
    m_pPixelCtrl(pPixelCtrl)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void PicViewCtrl::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
    wxGraphicsContext *gct = wxGraphicsContext::Create(dc);
    if(m_bFullRefresh)
    {
        m_rectRefresh.x = 0;
        m_rectRefresh.y = 0;
        m_rectRefresh.width  = m_cViewBitmap.GetWidth();
        m_rectRefresh.height = m_cViewBitmap.GetHeight();
    }
    Render(gc, gct);
    delete gc;
    delete gct;
}

void PicViewCtrl::Render(wxGraphicsContext* gc, wxGraphicsContext* gct)
{
    if(!m_bClearFlag)
    {
        gc->Scale(m_dScaleRate, m_dScaleRate);
        DrawBackGround(gc);
        m_pHRuler->SetTagValue(m_curLCUStart.x);
        m_pVRuler->SetTagValue(m_curLCUStart.y);
        gct->Scale(1, 1);
        gct->SetBrush(wxBrush(wxColor(255, 255, 255, 100)));
        gct->SetPen(wxPen(wxColor(255, 0, 0, 255)));
        gct->StrokeLine(0, m_curLCUStart.y*m_dScaleRate, m_curLCUStart.x*m_dScaleRate, m_curLCUStart.y*m_dScaleRate);
        gct->StrokeLine(m_curLCUStart.x*m_dScaleRate, 0, m_curLCUStart.x*m_dScaleRate, m_curLCUStart.y*m_dScaleRate);
        gct->SetPen(wxPen(wxColor(255, 255, 255, 100)));
        gct->DrawRectangle(m_curLCUStart.x*m_dScaleRate, m_curLCUStart.y*m_dScaleRate, (m_curLCUEnd.x - m_curLCUStart.x)*m_dScaleRate,
                           (m_curLCUEnd.y - m_curLCUStart.y)*m_dScaleRate);
    }
    else
        DrawNoPictureTips(gc);
}

void PicViewCtrl::SetBitmap(wxBitmap bitmap, wxBitmap bitmapY, wxBitmap bitmapU, wxBitmap bitmapV)
{
    m_bClearFlag   = false;
    m_cViewBitmap  = bitmap;
    m_cViewBitmapY = bitmapY;
    m_cViewBitmapU = bitmapU;
    m_cViewBitmapV = bitmapV;
    CalFitScaleRate();
    m_CtrlSize.SetWidth(m_dScaleRate*m_cViewBitmap.GetWidth());
    m_CtrlSize.SetHeight(m_dScaleRate*m_cViewBitmap.GetHeight());
    this->SetSizeHints(m_CtrlSize);
    GetParent()->FitInside();
    SetRulerCtrlFited();
    CalCurScrolledRectOnPicView(m_rectRefresh);
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
        SetFocus();
        wxPoint pt = event.GetPosition();
        if(event.Dragging() && event.LeftIsDown())
        {
            m_bFullRefresh = true;
            int xper, yper;
            wxPoint pos = ClientToScreen(pt);
            wxScrolledWindow* pPar = (wxScrolledWindow*)GetParent();
            pPar->GetScrollPixelsPerUnit(&xper, &yper);
            pPar->Scroll((m_delta.x - pos.x)/xper, (m_delta.y - pos.y)/yper);
            SetRulerCtrlFited();
        }
        m_bFullRefresh = false;
        wxPoint begin_org = m_curLCUStart;
        wxPoint end_org   = m_curLCUEnd;
        wxRect refreshRect;
        int posx = event.m_x/m_dScaleRate;
        int posy = event.m_y/m_dScaleRate;
        int id = GetCurLCURasterID(posx, posy);
        m_PosData._iBlockX      = m_curLCUStart.x;
        m_PosData._iBlockY      = m_curLCUStart.y;
        m_PosData._iBlockWidth  = m_curLCUEnd.x - m_curLCUStart.x;
        m_PosData._iBlockHeight = m_curLCUEnd.y - m_curLCUStart.y;
        m_PosData._iOffsetX     = posx % m_LCUSize.x;
        m_PosData._iOffsetY     = posy % m_LCUSize.y;
        wxCommandEvent event(wxEVT_POSITION_CHANGED, wxID_ANY);
        event.SetClientData(&m_PosData);
        wxPostEvent(m_pPixelCtrl, event);
        if(id != m_iLCURasterID)
        {
            CalCurScrolledRectOnPicView(m_rectRefresh);
            m_iLCURasterID = id;
            if(id != -1)
            {
                Refresh();
            }
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
        SetRulerCtrlFited();
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
    if(!m_bClearFlag && m_cViewBitmap.IsOk())
    {
        int width    = m_cViewBitmap.GetWidth();
        int height   = m_cViewBitmap.GetHeight();
        // get the screen size to cal min max scale rate
        int scrwidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
        int scrheight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
        double sratew = scrwidth/static_cast<double>(width);
        double srateh = scrheight/static_cast<double>(height);
        double srate  = sratew < srateh ? sratew : srateh;
        m_dMaxScaleRate  = 1.5*srate < 4.0 ? 4.0 : 1.5*srate;
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
    if(!m_bClearFlag && m_cViewBitmap.IsOk())
    {
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
        m_bFullRefresh = true;
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
    {
        event.Skip();
        return;
    }
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
    //int col_num = m_cViewBitmap.GetWidth()/cuw + (m_cViewBitmap.GetWidth()%cuw != 0);
    //int row_num = m_cViewBitmap.GetHeight()/cuh + (m_cViewBitmap.GetHeight()%cuh != 0);
    int startx = m_rectRefresh.x/cuw;
    int starty = m_rectRefresh.y/cuh;
    if(startx < 1) startx = 1;
    if(starty < 1) starty = 1;
    int width  = m_rectRefresh.x + m_rectRefresh.width;
    int height = m_rectRefresh.y + m_rectRefresh.height;
    int endx   = width/cuw + (width%cuw != 0);
    int endy   = height/cuh + (height%cuh != 0);
    gc->SetPen(*wxBLACK_PEN);
    for(int i = starty; i < endy; i++)
        gc->StrokeLine(0, i*cuh, m_cViewBitmap.GetWidth(), i*cuh);
    for(int i = startx; i < endx; i++)
        gc->StrokeLine(i*cuw, 0, i*cuw, m_cViewBitmap.GetHeight());
}

void PicViewCtrl::DrawBackGround(wxGraphicsContext* gc)
{
    switch(m_iShowWhich_O_Y_U_V)
    {
    case MODE_ORG:
        gc->DrawBitmap(m_cViewBitmap, 0, 0, m_cViewBitmap.GetWidth(), m_cViewBitmap.GetHeight());
        break;
    case MODE_Y:
        gc->DrawBitmap(m_cViewBitmapY, 0, 0, m_cViewBitmap.GetWidth(), m_cViewBitmap.GetHeight());
        break;
    case MODE_U:
        gc->DrawBitmap(m_cViewBitmapU, 0, 0, m_cViewBitmap.GetWidth(), m_cViewBitmap.GetHeight());
        break;
    case MODE_V:
        gc->DrawBitmap(m_cViewBitmapV, 0, 0, m_cViewBitmap.GetWidth(), m_cViewBitmap.GetHeight());
        break;
    default:
        assert(0);
        break;
    }

    if(m_bShowPUType)
    {
//        gc->SetPen(*wxTRANSPARENT_PEN);
//        gc->SetBrush(wxBrush(wxColor(255, 0, 0, 50)));
//        gc->DrawRectangle(0, 0, m_cViewBitmap.GetWidth(), m_cViewBitmap.GetHeight());
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
    SetRulerCtrlFited();
    wxBitmap::CleanUpHandlers();
}

void PicViewCtrl::SetPicYuvBuffer(TComPicYuv* pBuffer, const int w, const int h, const int bit)
{
    m_pBuffer = pBuffer;
    m_iYUVBit = bit;
    wxBitmap bmp(w, h, 24);
    wxBitmap bmpY(w, h, 24);
    wxBitmap bmpU(w, h, 24);
    wxBitmap bmpV(w, h, 24);
    g_tranformYUV2RGB(w, h, m_pBuffer, m_iYUVBit, bmp, bmpY, bmpU, bmpV, true);
    SetLCUSize(wxSize(64, 64));
    SetBitmap(bmp, bmpY, bmpU, bmpV);
    CalMinMaxScaleRate();
    wxCommandEvent event(wxEVT_YUVBUFFER_CHANGED, wxID_ANY);
    event.SetClientData(pBuffer);
    wxPostEvent(m_pPixelCtrl, event);
}

// pt2 is useless and it is a wrong position now
void PicViewCtrl::GetCurPicViewCtrlPosOnParent(wxPoint& pt1, wxPoint& pt2)
{
    wxSizer* pSizer = GetParent()->GetSizer();
    wxSizerItemList itemList = pSizer->GetChildren();

    assert(itemList.size() > 0);
    pt1 = (*itemList.begin())->GetPosition();
    // wrong cal
//    wxSize size = GetSize();
//    pt2.x = pt1.x + size.x;
//    pt2.y = pt1.y + size.y;
}

void PicViewCtrl::SetRulerCtrlFited()
{
    wxPoint pt1, pt2;
    GetCurPicViewCtrlPosOnParent(pt1, pt2);
    double startx, starty;
    startx = pt1.x;
    starty = pt1.y;
    if(!m_bFitMode || m_bClearFlag)
    {
        int xper, yper, x, y;
        wxScrolledWindow* pPar = (wxScrolledWindow*)GetParent();
        pPar->GetScrollPixelsPerUnit(&xper, &yper);
        pPar->GetViewStart(&x, &y);
        x *= xper;
        y *= yper;
        if(pt1.x >= 0 || pt1.y >= 0)
        {
            pt1.x  = ((pt1.x > 0) ? pt1.x : 0);
            pt1.y  = ((pt1.y > 0) ? pt1.y : 0);
            startx = pt1.x - x;
            starty = pt1.y - y;
        }
        else
        {
            startx = -x;
            starty = -y;
        }
    }
    m_pHRuler->SetStartPos(m_pHRuler->GetRulerWidth() + startx);
    m_pHRuler->SetStartValue(0);
    m_pVRuler->SetStartPos(starty);
    m_pVRuler->SetStartValue(0);
    if(m_bClearFlag)
    {
        m_pHRuler->SetEndValue(300);
        m_pVRuler->SetEndValue(300);
        m_pHRuler->SetScaleRate(1.0);
        m_pVRuler->SetScaleRate(1.0);
    }
    else
    {
        m_pHRuler->SetEndValue(m_cViewBitmap.GetWidth());
        m_pVRuler->SetEndValue(m_cViewBitmap.GetHeight());
        m_pHRuler->SetScaleRate(m_dScaleRate);
        m_pVRuler->SetScaleRate(m_dScaleRate);
    }
}

// get the real ScrolledRect to improve the efficiency
void PicViewCtrl::CalCurScrolledRectOnPicView(wxRect& rect)
{
    m_bFullRefresh = false;
    if(m_bFitMode || m_bClearFlag)
    {
        rect.x = 0;
        rect.y = 0;
        rect.width  = m_cViewBitmap.GetWidth();
        rect.height = m_cViewBitmap.GetHeight();
        return;
    }
    int startx, starty, endx, endy;
    wxPoint pt1, pt2;
    GetCurPicViewCtrlPosOnParent(pt1, pt2);
    int xper, yper, x, y;
    wxScrolledWindow* pPar = (wxScrolledWindow*)GetParent();
    pPar->GetScrollPixelsPerUnit(&xper, &yper);
    pPar->GetViewStart(&x, &y);
    wxSize parSize = pPar->GetSize();
    x *= xper;
    y *= yper;
    if(pt1.x >= 0 || pt1.y >= 0)
    {
        pt1.x  = ((pt1.x > 0) ? pt1.x : 0);
        pt1.y  = ((pt1.y > 0) ? pt1.y : 0);
        startx = pt1.x - x;
        starty = pt1.y - y;
    }
    else
    {
        startx = -x;
        starty = -y;
    }
    wxSize size = m_CtrlSize;
    endx = startx + size.x;
    endy = starty + size.y;
    endx = min(parSize.x, endx);
    endy = min(parSize.y, endy);
    if(startx > 0) startx = 0;
    else startx = -startx;
    if(starty > 0) starty = 0;
    else starty = -starty;
    rect.x = max(startx/m_dScaleRate - 4, 0.0);
    rect.y = max(starty/m_dScaleRate - 4, 0.0);
    rect.width = min(endx/m_dScaleRate + 4, (double)m_cViewBitmap.GetWidth());
    rect.height = min(endy/m_dScaleRate + 4, (double)m_cViewBitmap.GetHeight());
}

void PicViewCtrl::CalTwoRectsOutsideBox(wxRect& rect, const wxPoint& start, const wxPoint& end)
{
    int x2 = max(m_curLCUEnd.x, end.x);
    int y2 = max(m_curLCUEnd.y, end.y);
    rect.x = 0;
    rect.y = 0;
    rect.width = (x2+4)*m_dScaleRate;
    rect.height = (y2+4)*m_dScaleRate;
}

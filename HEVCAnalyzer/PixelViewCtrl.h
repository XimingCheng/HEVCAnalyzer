#ifndef PIXELVIEWCTRL_H_INCLUDED
#define PIXELVIEWCTRL_H_INCLUDED

#define TIMER_ID  wxID_HIGHEST+100

#include "HEVCAnalyzer.h"
#include "RulerCtrl.h"
#include "wx/timer.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_YUVBUFFER_CHANGED, wxID_ANY)
    DECLARE_EVENT_TYPE(wxEVT_POSITION_CHANGED, wxID_ANY)
END_DECLARE_EVENT_TYPES()

struct MSG_block_pos
{
    int _iBlockX;
    int _iBlockY;
    int _iBlockWidth;
    int _iBlockHeight;
    int _iOffsetX;
    int _iOffsetY;
};

class PixelViewCtrl : public wxScrolledWindow
{
public:
    DECLARE_DYNAMIC_CLASS(PixelViewCtrl);
    PixelViewCtrl(){}
    PixelViewCtrl(wxWindow *parent, wxWindowID id, RulerCtrl* pHRuler, RulerCtrl* pVRuler)
        : wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize,
        wxFULL_REPAINT_ON_RESIZE), m_FocusPos(10, 10),
        m_iCUHeight(32), m_iCUWidth(32), m_iHeightPerPixel(90),
        m_iWidthPerPixel(70), m_bHexFormat(true), m_bScrollMode(false),
        m_iXOffset(20), m_iYOffset(20), m_pBuffer(NULL), m_pBlockInfo(NULL),
        m_bPaintEventSource(false), m_pHRuler(pHRuler), m_pVRuler(pVRuler)
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        SetScrollRate(5, 7);
        m_pTimer = new wxTimer(this, TIMER_ID);
        m_pHRuler->SetAdaptiveMarkLen(false);
        m_pVRuler->SetAdaptiveMarkLen(false);
    }

private:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);
    void Render(wxDC& dc);
    void DrawGrid(wxDC& dc, int xstart, int ystart, int xend, int yend);
    void DrawFocusLine(wxDC& dc);
    void DrawBackground(wxDC& dc, int xstart, int ystart, int xend, int yend);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseMidUp(wxMouseEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    void OnLeftButtonDown(wxMouseEvent& event);
    void OnLeftButtonUp(wxMouseEvent& event);
    void LogicPosToIndex(int xLogic, int yLogic, int *xIndex, int *yIndex);
    void ShowOneCell(wxDC& dc, const int xIndex, const int yIndex);
    void AdaptiveSize(wxDC& dc);
    void OnBufferChanged(wxCommandEvent& event);
    void OnPosChanged(wxCommandEvent& event);
    void SetRulerCtrlFited();

public:
    void SetFocusPos(const wxPoint& pos);
    void SetCUHeight(const int height){m_iCUHeight = height;}
    void SetCUWidth(const int width){m_iCUWidth = width;}
    void SetHeight(const int height){m_iHeightPerPixel = height;}
    void SetWidth(const int width){m_iWidthPerPixel = width;}
    int  GetHeight()const{return m_iHeightPerPixel;}
    int  GetWidth()const{return m_iWidthPerPixel;}
    void SetHEXFormat(const bool b) { m_bHexFormat = b;}
    bool GetHEXFormat() const { return m_bHexFormat; }
    void Clear();

private:
    wxPoint         m_FocusPos;
    int             m_iCUHeight;
    int             m_iCUWidth;
    int             m_iHeightPerPixel;
    int             m_iWidthPerPixel;
    bool            m_bHexFormat;
    bool            m_bScrollMode;
    int             m_iXCurrentUnit;
    int             m_iYCurrentUnit;
    int             m_iXStep;
    int             m_iYStep;
    wxTimer*        m_pTimer;
    int             m_iXOffset;
    int             m_iYOffset;
    wxPoint         m_LeftDownPos;
    TComPicYuv*     m_pBuffer;
    MSG_block_pos*  m_pBlockInfo;
    /* true: outside, false: inside*/
    bool            m_bPaintEventSource;
    RulerCtrl*      m_pHRuler;
    RulerCtrl*      m_pVRuler;

    DECLARE_EVENT_TABLE();
};

#endif // PIXELVIEWCTRL_H_INCLUDED

#ifndef PICVIEWCTRL_H_INCLUDED
#define PICVIEWCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"

class PicViewCtrl : public wxControl
{
public:
    DECLARE_DYNAMIC_CLASS(PicViewCtrl);
    enum Direction
    {
        MOVE_UP    = 0,
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT,
    };

    PicViewCtrl() {}
    PicViewCtrl(wxWindow* parent, wxWindowID id, wxSimpleHtmlListBox* pList, wxFrame* pFrame)
        : wxControl (parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxWANTS_CHARS ),
        m_bClearFlag(true), m_bFitMode(true), m_dScaleRate(1.0), m_dMinScaleRate(0.1), m_dMaxScaleRate(2.0), m_dFitScaleRate(1.0),
        m_dScaleRateStep(0.02), m_delta(-1, -1), m_curLCUStart(-1, -1), m_curLCUEnd(-1, -1), m_iLCURasterID(-1), m_pList(pList),
        m_pFrame(pFrame), m_bShowGrid(true), m_bMouseWheelPageUpDown(false), m_bShowPUType(true)
    { SetBackgroundStyle(wxBG_STYLE_CUSTOM); }
    void SetScale(const double dScale);
    //void SetSize(const wxSize& size) { m_CtrlSize = size; }
    void SetBitmap(wxBitmap bitmap);
    void SetLCUSize(const wxSize& size);
    void SetClear(bool bClr = true) { m_bClearFlag = bClr; }
    void CalMinMaxScaleRate();
    void CalFitScaleRate();
    bool GetFitMode() const { return m_bFitMode; }
    void SetFitMode(const bool b) { m_bFitMode = b; }
    bool IsShowGrid() const { return m_bShowGrid; }
    void SetShowGrid(const bool b) { m_bShowGrid = b; }
    bool IsMouseWheelPageUpDown() const { return m_bMouseWheelPageUpDown; }
    void SetMouseWheelPageUpDown(const bool b) { m_bMouseWheelPageUpDown = b; }
    bool IsShowPUType() const { return m_bShowPUType; }
    void SetShowPUType(const bool b) { m_bShowPUType = b; }

private:
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLButtonDown(wxMouseEvent& event);
    void OnMouseLButtonUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    void Render(wxGraphicsContext* gc);
    void ChangeScaleRate(const double rate);
    int  GetCurLCURasterID(const double x, const double y);
    void MoveLCURect(const Direction& d);
    bool ShowPageByDiffNumber(const int diff);
    void CalStartEndPointByLCUId(const int id);
    void DrawBackGround(wxGraphicsContext* gc);
    void DrawNoPictureTips(wxGraphicsContext* gc);
    void DrawGrid(wxGraphicsContext* gc);

private:
    bool                 m_bClearFlag;
    bool                 m_bFitMode;
    double               m_dScaleRate;
    double               m_dMinScaleRate;
    double               m_dMaxScaleRate;
    double               m_dFitScaleRate;
    double               m_dScaleRateStep;
    wxSize               m_CtrlSize;
    wxSize               m_LCUSize;
    wxBitmap             m_cViewBitmap;
    wxPoint              m_delta;
    // Mouse moving LCU location
    wxPoint              m_curLCUStart;
    wxPoint              m_curLCUEnd;
    int                  m_iLCURasterID;
    wxSimpleHtmlListBox* m_pList;
    wxFrame*             m_pFrame;
    bool                 m_bShowGrid;
    bool                 m_bMouseWheelPageUpDown;
    bool                 m_bShowPUType;

    DECLARE_EVENT_TABLE();
};

#endif // PICVIEWCTRL_H_INCLUDED

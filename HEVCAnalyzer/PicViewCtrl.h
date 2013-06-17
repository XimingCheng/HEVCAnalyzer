#ifndef PICVIEWCTRL_H_INCLUDED
#define PICVIEWCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"
#include "RulerCtrl.h"

class PicViewCtrl : public wxControl
{
public:
    DECLARE_DYNAMIC_CLASS(PicViewCtrl);
    enum Direction
    {
        MOVE_UP  = 0,
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT,
    };

    enum ShowMode
    {
        MODE_ORG = 0,
        MODE_Y,
        MODE_U,
        MODE_V,
    };

    PicViewCtrl() {}
    PicViewCtrl(wxWindow* parent, wxWindowID id, wxSimpleHtmlListBox* pList, RulerCtrl* pHRuler, RulerCtrl* pVRuler, wxFrame* pFrame)
        : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxWANTS_CHARS),
        m_bClearFlag(true), m_bFitMode(true), m_dScaleRate(1.0), m_dMinScaleRate(0.1), m_dMaxScaleRate(2.0), m_dFitScaleRate(1.0),
        m_dScaleRateStep(0.02), m_delta(-1, -1), m_curLCUStart(-1, -1), m_curLCUEnd(-1, -1), m_iLCURasterID(-1), m_pList(pList),
        m_pFrame(pFrame), m_bShowGrid(true), m_bMouseWheelPageUpDown(false), m_bShowPUType(true), m_pBuffer(NULL),
        m_iYUVBit(8), m_iShowWhich_O_Y_U_V(MODE_ORG), m_pHRuler(pHRuler), m_pVRuler(pVRuler)
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    }

    void SetScale(const double dScale);
    //void SetSize(const wxSize& size) { m_CtrlSize = size; }
    void SetBitmap(wxBitmap bitmap, wxBitmap bitmapY, wxBitmap bitmapU, wxBitmap bitmapV);
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
    int  WhichTobeShown() const { return m_iShowWhich_O_Y_U_V; }
    void SetWhichTobeShown(const ShowMode& which) { m_iShowWhich_O_Y_U_V = which; }
    void SetPicYuvBuffer(TComPicYuv* pBuffer, const int w, const int h, const int bit);
    void SetRulerCtrlFited();

private:
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLButtonDown(wxMouseEvent& event);
    void OnMouseLButtonUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    void Render(wxGraphicsContext* gc, wxGraphicsContext* gct);
    void ChangeScaleRate(const double rate);
    int  GetCurLCURasterID(const double x, const double y);
    void MoveLCURect(const Direction& d);
    bool ShowPageByDiffNumber(const int diff);
    void CalStartEndPointByLCUId(const int id);
    void DrawBackGround(wxGraphicsContext* gc);
    void DrawNoPictureTips(wxGraphicsContext* gc);
    void DrawGrid(wxGraphicsContext* gc);
    void GetCurPicViewCtrlPosOnParent(wxPoint& pt1, wxPoint& pt2);

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
    wxBitmap             m_cViewBitmapY;
    wxBitmap             m_cViewBitmapU;
    wxBitmap             m_cViewBitmapV;
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
    TComPicYuv*          m_pBuffer;
    int                  m_iYUVBit;
    ShowMode             m_iShowWhich_O_Y_U_V;  //!< 0 - original 1 - Y 2 - U 3 - V
    RulerCtrl*           m_pHRuler;
    RulerCtrl*           m_pVRuler;

    DECLARE_EVENT_TABLE();
};

#endif // PICVIEWCTRL_H_INCLUDED

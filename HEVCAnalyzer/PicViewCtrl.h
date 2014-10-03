#ifndef PICVIEWCTRL_H_INCLUDED
#define PICVIEWCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"
#include "RulerCtrl.h"
#include "PixelViewCtrl.h"
#include "MainUIInstance.h"
#include <list>
#include <vector>
#include <wx/dnd.h>

class HEVCStatusBar;
class DragDropFile;

enum ShowMode
{
    MODE_ORG = 0,
    MODE_Y,
    MODE_U,
    MODE_V,
};

class PicViewCtrl : public wxControl //, public wxFileDropTarget
{
public:
    DECLARE_DYNAMIC_CLASS(PicViewCtrl);
//    DECLARE_DYNAMIC_CLASS(wxFileDropTarget);
    enum Direction
    {
        MOVE_UP  = 0,
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT,
    };

    PicViewCtrl() {}
    PicViewCtrl(wxWindow* parent, wxWindowID id, wxSimpleHtmlListBox* pList, RulerCtrl* pHRuler, RulerCtrl* pVRuler,
                wxWindow* pPixelWin, wxFrame* pFrame);
    ~PicViewCtrl();

    void SetScale(const double dScale);
    void SetPicWidth(const int w) { m_iPicWidth = w; }
    void SetPicHeight(const int h) { m_iPicHeight = h; }
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
    void Clear();
    bool ShowPageByDiffNumber(const int diff, bool bWantRet = false);
    void SetHEVCStatusBar(HEVCStatusBar* bar) { m_pStatusBar = bar; }
    void SetCurSliderInStatusBarPos();
    //bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
    void OnDropFiles(wxDropFilesEvent& event);
    void ChangeScaleRate(const double rate);
    void SetRowData(const int num_row, const int* pRowData);
    void SetColData(const int num_col, const int* pColData);
    void SetOpenedIsYUVfile(const bool b) { m_bOpenedYUVfile = b; }
    void SetSplitData(const int size, const PtInfo* pData);

private:
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLButtonDown(wxMouseEvent& event);
    void OnMouseLButtonUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBkg(wxEraseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    /* the  drop and drag action function */

    void Render(wxGraphicsContext* gc, wxGraphicsContext* gct);
    int  GetCurLCURasterID(const double x, const double y, wxPoint& start, wxPoint& end);
    void MoveLCURect(const Direction& d);
    void CalStartEndPointByLCUId(const int id, wxPoint& start, wxPoint& end);
    void DrawBackGround(wxGraphicsContext* gc, wxGraphicsContext* gct);
    void DrawNoPictureTips(wxGraphicsContext* gc);
    void DrawGrid(wxGraphicsContext* gc);
    void GetCurPicViewCtrlPosOnParent(wxPoint& pt1, wxPoint& pt2);
    void CalCurScrolledRectOnPicView(wxRect& rect);
    void CalTwoRectsOutsideBox(wxRect& rect, const wxPoint& start, const wxPoint& end);
    void DrawTilesGrid(wxGraphicsContext* gc);
    void DrawSplitInfo(wxGraphicsContext* gc);

private:
    // the width and height are used in the m_cViewBitmap is invalid
    int                  m_iPicWidth;
    int                  m_iPicHeight;
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
    bool                 m_bOpenedYUVfile;
    bool                 m_bShowTilesInfo;
    bool                 m_bMouseWheelPageUpDown;
    bool                 m_bShowPUType;
    TComPicYuv*          m_pBuffer;
    int                  m_iYUVBit;
    ShowMode             m_iShowWhich_O_Y_U_V;  //!< 0 - original 1 - Y 2 - U 3 - V
    RulerCtrl*           m_pHRuler;
    RulerCtrl*           m_pVRuler;
    wxRect               m_rectRefresh;
    bool                 m_bFullRefresh;
    wxWindow*            m_pPixelCtrl;
    MSG_block_pos        m_PosData;
    int                  m_iSelectedLCUId;
    wxPoint              m_curSelLCUStart;
    wxPoint              m_curSelLCUEnd;
    HEVCStatusBar*       m_pStatusBar;
    DragDropFile*        m_pDragDropFile;

    // tiles info
    int                  m_iNumRow;
    int                  m_iNumCol;
    int*                 m_piRowData;
    int*                 m_piColData;

    // cu split info, the first index of vector is CU index
    vector<vector<PtInfo> > m_vCurCUSplitInfo;
    vector<vector<PtInfo> > m_vCurPUSplitInfo;
    vector<vector<PtInfo> > m_vCurTUSplitInfo;

    DECLARE_EVENT_TABLE();
};

class DragDropFile : public wxFileDropTarget
{
public:
    void setFrameWindow(wxFrame* pFrame) { m_pFrame = pFrame; }
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

private:
    wxFrame*             m_pFrame;
};

#endif // PICVIEWCTRL_H_INCLUDED

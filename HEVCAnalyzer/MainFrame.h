#ifndef MAINFRAME_H_INCLUDED
#define MAINFRAME_H_INCLUDED

#include <list>
#include "HEVCAnalyzer.h"
#include "ThumbnailThread.h"
#include "PicViewCtrl.h"
#include "PixelViewCtrl.h"
#include "RulerCtrl.h"

DECLARE_EVENT_TYPE(wxEVT_ADDANIMAGE_THREAD, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_END_THREAD, wxID_ANY)

class ThumbnailThread;
class CenterPageManager;

class MainFrame : public wxFrame
{
public:
    enum
    {
        ID_NoteBookLeftPane = wxID_HIGHEST+1,
        ID_NoteBookBottomPane,
        ID_NoteBookCenterPane,
        ID_SwitchGrid,
        ID_SwitchColorYUV,
        ID_Switch_YUV,
        ID_Switch_Y,
        ID_Switch_U,
        ID_Switch_V,
    };

    MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
    ~MainFrame();

private:
    void           OnExit(wxCommandEvent& evt);
    void           OnThreadAddImage(wxCommandEvent& event);
    void           OnMainFrameSizeChange(wxSizeEvent& event);
    void           OnIdle(wxIdleEvent& event);
    // UI creator functions
    void           CreateMenuToolBar();
    void           CreateFileIOToolBar();
    void           CreateYUVToolBar();
    void           CreateNoteBookPane();
    wxNotebook*    CreateLeftNotebook();
    wxNotebook*    CreateCenterNotebook();
    wxNotebook*    CreateBottomNotebook();
    void           SetColorComponent();
    void           ClearThumbnalMemory();
    void           InitThumbnailListView();
    // File IO operators
    void           OnOpenFile(wxCommandEvent& event);
    void           OnCloseFile(wxCommandEvent& event);
    void           OnThumbnailLboxSelect(wxCommandEvent& event);
    void           OnThreadEnd(wxCommandEvent& event);
    void           OnUpdateUI(wxUpdateUIEvent& event);
    void           OnDropDownToolbarYUV(wxAuiToolBarEvent& event);
    void           OnSwitchShowGrid(wxCommandEvent& event);
    void           OnSwitchYUV(wxCommandEvent& event);

private:
    wxAuiManager         m_mgr;
    wxSimpleHtmlListBox* m_pThumbnalList;
    wxImageList*         m_pImageList;
    wxArrayString        m_StrMemFileName;
//    wxScrolledWindow*    m_pDecodeScrolledWin;
//    PicViewCtrl*         m_pPicViewCtrl;
//    RulerCtrl*           m_pPicHRuler;
//    RulerCtrl*           m_pPicVRuler;
    wxTextCtrl*          m_pTCLogWin;
    PixelViewCtrl*       m_pPixelViewCtrl;
    RulerCtrl*           m_pPixelHRuler;
    RulerCtrl*           m_pPixelVRuler;

    long                 m_notebook_style;
    long                 m_notebook_theme;
    bool                 m_bYUVFile;
    bool                 m_bOPened;
    ShowMode             m_eYUVComponentChoose;

    int                  m_iSourceWidth;
    int                  m_iSourceHeight;
    int                  m_iYUVBit;
    wxFileOffset         m_FileLength;

    TVideoIOYuv          m_cYUVIO;
    TComPicYuv*          m_pcPicYuvOrg;
    ThumbnailThread*     m_pThumbThread;
    CenterPageManager*   m_pCenterPageManager;

    wxAuiToolBar*        m_ioToolBar;
    wxAuiToolBar*        m_yuvToolBar;

    DECLARE_EVENT_TABLE()
};

struct PanelElments
{
    wxPanel*          _pPicPanel;
    wxScrolledWindow* _pDecodeScrolledWin;
    PicViewCtrl*      _pPicViewCtrl;
    RulerCtrl*        _pPicHRuler;
    RulerCtrl*        _pPicVRuler;
    wxString          _name;
};

class CenterPageManager
{
public:
    CenterPageManager(wxNotebook* pNoteBook, wxSimpleHtmlListBox* pList, wxFrame* pMainFrame,
                     wxWindow* pPixelViewCtrl)
    : m_pCenterNoteBook(pNoteBook), m_pMainFrame(pMainFrame),
    m_pPixelViewCtrl(pPixelViewCtrl), m_pList(pList), m_bFirstCreate(true)
    {}
    ~CenterPageManager();

    void                     Destory();
    void                     Clear();
    void                     Close();
    void                     Show();
    void                     InsertNewPage(const int insertAt, const wxString& name);
    inline unsigned int      GetSize() const { return m_PageList.size(); }
    inline PicViewCtrl*      GetPicViewCtrl(const std::size_t index);
    inline wxScrolledWindow* GetDecodeScrolledWin(const std::size_t index);
    inline RulerCtrl*        GetHorRulerCtrl(const std::size_t index);
    inline RulerCtrl*        GetVerRulerCtrl(const std::size_t index);
    inline wxString          GetName(const std::size_t index);

private:
    std::list<PanelElments>  m_PageList;
    wxNotebook*              m_pCenterNoteBook;
    wxFrame*                 m_pMainFrame;
    wxWindow*                m_pPixelViewCtrl;
    wxSimpleHtmlListBox*     m_pList;
    bool                     m_bFirstCreate;
};

#endif // MAINFRAME_H_INCLUDED

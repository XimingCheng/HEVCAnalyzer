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
class CenterPageManger;

class MainFrame : public wxFrame
{
public:
    enum
    {
        ID_NoteBookLeftPane = wxID_HIGHEST+1,
        ID_NoteBookBottomPane,
        ID_NoteBookCenterPane,
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
    void           CreateNoteBookPane();
    wxNotebook*    CreateLeftNotebook();
    wxNotebook*    CreateCenterNotebook();
    wxNotebook*    CreateBottomNotebook();
    // File IO operators
    void           OnOpenFile(wxCommandEvent& event);
    void           OnCloseFile(wxCommandEvent& event);

    void           OnThumbnailLboxSelect(wxCommandEvent& event);
    void           OnThreadEnd(wxCommandEvent& event);
    void           ClearThumbnalMemory();
    void           InitThumbnailListView();

private:
    wxAuiManager         m_mgr;
    wxSimpleHtmlListBox* m_pThumbnalList;
    wxImageList*         m_pImageList;
    wxArrayString        m_StrMemFileName;
    wxScrolledWindow*    m_pDecodeScrolledWin;
    PicViewCtrl*         m_pPicViewCtrl;
    RulerCtrl*           m_pPicHRuler;
    RulerCtrl*           m_pPicVRuler;
    wxTextCtrl*          m_pTCLogWin;
    PixelViewCtrl*       m_pPixelViewCtrl;
    RulerCtrl*           m_pPixelHRuler;
    RulerCtrl*           m_pPixelVRuler;

    long                 m_notebook_style;
    long                 m_notebook_theme;
    bool                 m_bYUVFile;
    bool                 m_bOPened;

    int                  m_iSourceWidth;
    int                  m_iSourceHeight;
    int                  m_iYUVBit;
    wxFileOffset         m_FileLength;

    TVideoIOYuv          m_cYUVIO;
    TComPicYuv*          m_pcPicYuvOrg;
    ThumbnailThread*     m_pThumbThread;

    DECLARE_EVENT_TABLE()
};

struct PanelElments
{
    PixelViewCtrl*  _pPixelViewCtrl;
    RulerCtrl*      _pPicHRuler;
    RulerCtrl*      _pPicVRuler;
    wxString        _name;
};

class CenterPageManger
{
public:
    CenterPageManger(wxNotebook* pNoteBook) {}
    ~CenterPageManger();

    void           Destory();
    void           Show();
    void           InsertNewPage(const int insertAt, const wxString& name);
    unsigned int   GetSize() const { return m_PageList.size(); }
    PixelViewCtrl* GetPixelViewCtrl(const int index);
    RulerCtrl*     GetHorRulerCtrl(const int index);
    RulerCtrl*     GetVerRulerCtrl(const int index);
    wxString       GetName(const int index);

private:
    std::list<PanelElments> m_PageList;
};

#endif // MAINFRAME_H_INCLUDED

#ifndef MAINFRAME_H_INCLUDED
#define MAINFRAME_H_INCLUDED

#include "HEVCAnalyser.h"
#include "../TLibVideoIO/TVideoIOYuv.h"

DECLARE_EVENT_TYPE(wxEVT_ADDANIMAGE_THREAD, wxID_ANY)

class MainFrame : public wxFrame
{
public:
    enum
    {
        ID_NoteBookLeftPane = wxID_HIGHEST+1,
        ID_NoteBookBottomPane,
        ID_NoteBookCenterPane,
        ID_OPEN_FILE,
        ID_CLOSE_FILE,
    };

    MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
    ~MainFrame();

private:
    void           OnExit(wxCommandEvent& evt);
    void           OnThreadAddImage(wxCommandEvent& event);
    // UI creator functions
    void           CreateMenuToolBar();
    void           CreateNoteBookPane();
    wxNotebook*    CreateLeftNotebook();
    // File IO operators
    void           OnOpenFile(wxCommandEvent& event);
    void           OnCloseFile(wxCommandEvent& event);

    void           AddThumbnailListSingleThread();

private:
    wxAuiManager   m_mgr;
    wxNotebook*    m_pLeftNoteBook;
    wxPanel*       m_pThumbnalListPanel;
    wxListCtrl*    m_pThumbnalList;

    long           m_notebook_style;
    long           m_notebook_theme;
    bool           m_bYUVFile;
    bool           m_bOPened;

    int            m_iSourceWidth;
    int            m_iSourceHeight;

    TVideoIOYuv    m_cYUVIO;

    DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H_INCLUDED

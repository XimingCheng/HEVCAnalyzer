#include "MainFrame.h"
#include "YUVConfigDlg.h"
#include "HEVCAnalyzer.h"

enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{

    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

DEFINE_EVENT_TYPE(wxEVT_ADDANIMAGE_THREAD)
DEFINE_EVENT_TYPE(wxEVT_END_THREAD)

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpenFile)
    EVT_MENU(wxID_CLOSE, MainFrame::OnCloseFile)
    EVT_COMMAND(wxID_ANY, wxEVT_ADDANIMAGE_THREAD, MainFrame::OnThreadAddImage)
    EVT_COMMAND(wxID_ANY, wxEVT_END_THREAD, MainFrame::OnThreadEnd)
    EVT_SIZE(MainFrame::OnMainFrameSizeChange)
    EVT_IDLE(MainFrame::OnIdle)
    EVT_LISTBOX(wxID_ANY, MainFrame::OnThumbnailLboxSelect)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style)
            : wxFrame(parent, id, title, pos, size, style),
            m_pPicViewCtrl(NULL), m_bYUVFile(false), m_bOPened(false), m_pcPicYuvOrg(NULL), m_pThumbThread(NULL)
{
    m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
    m_mgr.SetManagedWindow(this);
    // this is not define yet
    // SetIcon(wxIcon(sample_xpm));
    m_notebook_style = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS
                    | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
    m_notebook_theme = 1;
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    CreateMenuToolBar();
    CreateNoteBookPane();

    Centre();
    g_LogMessage(_T("HEVC Analyzer load sucessfully"));

    m_mgr.Update();
}

void MainFrame::CreateMenuToolBar()
{
    wxMenuBar* mb = new wxMenuBar;
    wxMenu* file_menu = new wxMenu;
    file_menu->Append(wxID_OPEN, _T("Open File"));
    file_menu->Append(wxID_CLOSE, _T("Close Current File"));
    file_menu->Append(wxID_EXIT, _T("Exit"));
    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _T("About HEVC Analyzer"));

    mb->Append(file_menu, _T("File"));
    mb->Append(help_menu, _T("Help"));

    SetMenuBar(mb);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(_T("Ready"));
    SetMinSize(wxSize(400, 300));

    wxAuiToolBar* tb = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    tb->SetToolBitmapSize(wxSize(16, 16));
    wxBitmap tb_open = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 16));
    tb->AddTool(wxID_OPEN, wxT("Open File"), tb_open, wxT("Open File"), wxITEM_NORMAL);
    tb->Realize();

    m_mgr.AddPane(tb, wxAuiPaneInfo().
                  Name(_T("tb")).Caption(_T("Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));
    g_uiMaxCUWidth  = 64;
    g_uiMaxCUHeight = 64;
}

void MainFrame::CreateNoteBookPane()
{
    m_mgr.AddPane(CreateLeftNotebook(), wxAuiPaneInfo().Name(_T("Left NoteBook")).Caption(_T("YUV info")).
                  BestSize(wxSize(300,100)).MaxSize(wxSize(500,100)).Left().Layer(1));
    m_mgr.AddPane(CreateCenterNotebook(), wxAuiPaneInfo().Name(_T("Center NoteBook")).Center().Layer(0));
//    m_mgr.AddPane(new wxTextCtrl(this,wxID_ANY, _T("test"),
//                          wxPoint(0,0), wxSize(150,90),
//                          wxNO_BORDER | wxTE_MULTILINE), wxAuiPaneInfo().Name(_T("Bottom NoteBook")).Bottom().Layer(1));
    m_mgr.AddPane(CreateBottomNotebook(), wxAuiPaneInfo().Name(_T("Bottom NoteBook")).Bottom().Layer(1));
}

MainFrame::~MainFrame()
{
    ClearThumbnalMemory();
    m_mgr.UnInit();
}

void MainFrame::OnExit(wxCommandEvent& evt)
{
    Close(true);
}

wxNotebook* MainFrame::CreateBottomNotebook()
{
    wxNotebook* ctrl = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize(460,200), 0 );
    wxGridSizer* gSizer = new wxGridSizer( 1, 0, 0 );
    wxPanel* pLogPanel = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_pTCLogWin= new wxTextCtrl(pLogPanel, wxID_ANY, _T(""), wxPoint(0, 0), wxSize(150, 90), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxHSCROLL);
    g_SetActiveTarget(m_pTCLogWin);

    gSizer->Add(m_pTCLogWin, 0, wxEXPAND, 5 );
    pLogPanel->SetSizer( gSizer );
    pLogPanel->Layout();
    ctrl->AddPage( pLogPanel, _T("Log Window"), true );
    wxPanel* m_panel7 = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    ctrl->AddPage( m_panel7, _T("Other information"), false );
    g_LogMessage(_T("Message"));
    g_LogError(_T("Error"));
    g_LogWarning(_T("Warning"));
    return ctrl;
}
wxNotebook* MainFrame::CreateLeftNotebook()
{
    wxNotebook* ctrl = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize(460,200), 0 );
    wxGridSizer* gSizer = new wxGridSizer( 1, 0, 0 );
    wxPanel* pThumbnalListPanel = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_pThumbnalList = new wxSimpleHtmlListBox(pThumbnalListPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    gSizer->Add( m_pThumbnalList, 0, wxEXPAND, 5 );
    pThumbnalListPanel->SetSizer( gSizer );
    pThumbnalListPanel->Layout();
    ctrl->AddPage( pThumbnalListPanel, _T("Thumbnails"), true );
    wxPanel* m_panel7 = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    ctrl->AddPage( m_panel7, _T("CU Pixels"), false );
    return ctrl;
}

wxNotebook* MainFrame::CreateCenterNotebook()
{
    wxNotebook* ctrl = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize(460,200), 0 );
    wxGridSizer* gSizer = new wxGridSizer( 1, 0, 0 );
    wxPanel* pDecodePanel = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    //wxScrolledWindow
    m_pDecodeScrolledWin = new wxScrolledWindow( pDecodePanel, -1, wxDefaultPosition, wxDefaultSize, wxScrolledWindowStyle);
    m_pPicViewCtrl = new PicViewCtrl(m_pDecodeScrolledWin, wxID_ANY, m_pThumbnalList, this);
    m_pPicViewCtrl->SetSizeHints(300, 300);
    wxGridSizer* innerSizer = new wxGridSizer( 1, 0, 0 );
    innerSizer->Add(m_pPicViewCtrl, 1, wxALIGN_CENTER);

    m_pDecodeScrolledWin->SetScrollRate( 5, 5 );
    m_pDecodeScrolledWin->SetSizer( innerSizer );
    gSizer->Add( m_pDecodeScrolledWin, 1, wxEXPAND );
    pDecodePanel->SetSizer( gSizer );
    pDecodePanel->Layout();
    ctrl->AddPage( pDecodePanel, _T("Decode Pic"), true );
    wxPanel* m_panel7 = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    ctrl->AddPage( m_panel7, _T("Residual Pic"), false );
    return ctrl;
}

void MainFrame::OnOpenFile(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("Open YUV file or HEVC stream file"), _T(""), _T(""),
                     _T("YUV files (*.yuv)|*.yuv"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;
    wxString sfile = dlg.GetPath();
 //   if(sfile.EndsWith(_T(".yuv")) || sfile.EndsWith(_T(".YUV")))
    if(sfile.Lower().EndsWith(_T(".yuv")))
        m_bYUVFile = true;
    else
    {
        m_bYUVFile = false;
        g_LogError(_T("The file to be open must be YUV file"));
    }

    if(m_bYUVFile)
    {
        YUVConfigDlg cdlg(this);
        if(cdlg.ShowModal() == wxID_CANCEL)
            return;
        // multi-thread
        OnCloseFile(event);
        m_bOPened = true;
        m_iSourceWidth = cdlg.GetWidth();
        m_iSourceHeight = cdlg.GetHeight();
        m_pPicViewCtrl->SetScale(1.0);
        m_iYUVBit = (cdlg.Is10bitYUV() ? 10 : 8);
        m_FileLength = wxFile((const wxChar*)sfile).Length();
        m_pPicViewCtrl->SetFitMode(true);
        m_cYUVIO.open((char *)sfile.mb_str(wxConvUTF8).data(), false, m_iYUVBit, m_iYUVBit, m_iYUVBit, m_iYUVBit);
        m_pcPicYuvOrg = new TComPicYuv;
        m_pcPicYuvOrg->create( m_iSourceWidth, m_iSourceHeight, 64, 64, 4 );
        double scaleRate = 165.0/m_iSourceWidth;
        InitThumbnailListView();
        g_LogMessage(_T("Initialize thumbnail finished"));
        m_pImageList = new wxImageList((int)m_iSourceWidth*scaleRate, (int)m_iSourceHeight*scaleRate);
        m_pThumbThread = new ThumbnailThread(this, m_pImageList, m_iSourceWidth, m_iSourceHeight, m_iYUVBit, sfile);
        if(m_pThumbThread->Create() != wxTHREAD_NO_ERROR)
        {
            g_LogError(wxT("Can't create the thread!"));
            delete m_pThumbThread;
            m_pThumbThread = NULL;
        }
        else
        {
            if(m_pThumbThread->Run() != wxTHREAD_NO_ERROR)
            {
                g_LogError(wxT("Can't create the thread!"));
                delete m_pThumbThread;
                m_pThumbThread = NULL;
            }
        }

    }
    else
    {
        OnCloseFile(event);
        m_bOPened = true;
    }
}

void MainFrame::OnCloseFile(wxCommandEvent& event)
{
    if(m_bOPened)
    {
        if(m_bYUVFile)
        {
            m_cYUVIO.close();
            if(m_pThumbThread)
            {
                if(m_pThumbThread->IsRunning())
                    m_pThumbThread->Delete();
                m_pThumbThread = NULL;
            }
            if(m_pcPicYuvOrg)
            {
                m_pcPicYuvOrg->destroy();
                delete m_pcPicYuvOrg;
                m_pcPicYuvOrg = NULL;
            }
            m_pPicViewCtrl->SetClear();
            m_pPicViewCtrl->Refresh();
            m_pImageList->RemoveAll();
            if(m_StrMemFileName.GetCount())
                ClearThumbnalMemory();
            m_FileLength = 0;
            g_ClearLog();
        }
        else
        {

        }
        m_bOPened = false;
    }
}

void MainFrame::OnThreadAddImage(wxCommandEvent& event)
{
    int frame = event.GetInt();
    long framenumber = event.GetExtraLong();
//    wxArrayString arr;
    g_LogMessage(wxString::Format(_T("Add some images frome %d to %d"), frame-framenumber+1, frame));
    for(int i = 0;  i < (int)framenumber; i++)
    {
        int tmp = frame-framenumber+i+1;
        wxString filename = wxString::Format(_T("poc %d.bmp"), tmp);
        m_StrMemFileName.Add(filename);
        wxMemoryFSHandler::AddFile(wxString::Format(_T("poc %d.bmp"), tmp), m_pImageList->GetBitmap(tmp),wxBITMAP_TYPE_BMP);
        wxString label = wxString::Format(_T("<span>&nbsp;</span><p align=\"center\"><img src=\"memory:poc %d.bmp\"><br></p><span text-align=center>POC %d </span><br>"), tmp, tmp);
//        m_pThumbnalList->Insert(label, tmp);
//        m_pThumbnalList->Delete(tmp+1);
//       arr.Add(label);
        m_pThumbnalList->SetString(tmp, label);
    }
    m_pThumbnalList->Freeze();
    unsigned int cnt = m_pThumbnalList->GetFirstVisibleLine();
//   m_pThumbnalList->Append(arr);
    m_pThumbnalList->ScrollToLine(cnt);
    m_pThumbnalList->Thaw();
    m_pThumbnalList->RefreshAll();
    if(frame == framenumber-1)
    {
        event.SetInt(0);
        OnThumbnailLboxSelect(event);
        m_pThumbnalList->SetSelection(0);
        m_pThumbnalList->SetFocus();
    }
}

void MainFrame::OnThreadEnd(wxCommandEvent& event)
{
    m_pThumbThread = NULL;
    m_pImageList->RemoveAll();
}

void MainFrame::ClearThumbnalMemory()
{
    for(int i = 0; i < (int)m_StrMemFileName.GetCount(); i++)
        wxMemoryFSHandler::RemoveFile(m_StrMemFileName[i]);
    m_StrMemFileName.Clear();
    m_pThumbnalList->Clear();
    m_pThumbnalList->RefreshAll();
}

void MainFrame::OnThumbnailLboxSelect(wxCommandEvent& event)
{
    wxBusyCursor wait;
    int frame = event.GetInt();
    m_cYUVIO.reset();
    m_cYUVIO.skipFrames(frame, m_iSourceWidth, m_iSourceHeight);
    int pad[] = {0, 0};
    m_cYUVIO.read(m_pcPicYuvOrg, pad);
    wxBitmap bmp(m_iSourceWidth, m_iSourceHeight, 24);
    g_tranformYUV2RGB(m_iSourceWidth, m_iSourceHeight, m_pcPicYuvOrg, m_iYUVBit, bmp);
    m_pPicViewCtrl->SetLCUSize(wxSize(64, 64));
    m_pPicViewCtrl->CalMinMaxScaleRate();
    m_pPicViewCtrl->SetBitmap(bmp);
}
void MainFrame::InitThumbnailListView()
{
    int framenumber = m_FileLength/(m_iSourceWidth*m_iSourceHeight*1.5*(m_iYUVBit==10?2:1));
    g_LogMessage(wxString::Format(_T("frame numbes: %d\n"), framenumber));
    wxArrayString arr;
    for(int i = 0;  i < framenumber; i++)
    {
        wxString label = wxString::Format(_T("<span>&nbsp;</span><p align=\"center\"><img src=\"/home/luqingbo/Downloads/horse.bmp\"><br></p><span text-align=center>POC %d </span><br>"), i);
        arr.Add(label);
    }
    m_pThumbnalList->Append(arr);
}


void MainFrame::OnMainFrameSizeChange(wxSizeEvent& event)
{
    if(m_pPicViewCtrl)
    {
        m_pPicViewCtrl->CalFitScaleRate();
    }
}

void MainFrame::OnIdle(wxIdleEvent& event)
{
    if(m_pPicViewCtrl)
    {
        m_pPicViewCtrl->CalFitScaleRate();
        event.RequestMore(false);
    }
}


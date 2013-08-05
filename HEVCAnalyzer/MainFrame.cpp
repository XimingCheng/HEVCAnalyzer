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
    EVT_MENU(ID_SwitchGrid, MainFrame::OnSwitchShowGrid)
    EVT_MENU_RANGE(ID_Switch_YUV, ID_Switch_V, MainFrame::OnSwitchYUV)
    EVT_MENU(ID_SwitchfitMode, MainFrame::OnSwitchFitMode)
    EVT_MENU(ID_SwitchHEXPixel, MainFrame::OnSwitchHEXPixel)
    EVT_MENU(ID_GoToNextFrame, MainFrame::OnGoToNextFrame)
    EVT_MENU(ID_GoToPreFrame, MainFrame::OnGoToPreFrame)
    EVT_MENU(ID_Play_Pause, MainFrame::OnPlayorPause)
    EVT_MENU(ID_FastForward, MainFrame::OnFastForward)
    EVT_MENU(ID_FastBackward, MainFrame::OnFastBackward)
    EVT_MENU(ID_ReOpenWrongConfigYUVFile, MainFrame::OnReOpenWrongConfigYUVFile)
    EVT_COMMAND(wxID_ANY, wxEVT_ADDANIMAGE_THREAD, MainFrame::OnThreadAddImage)
    EVT_COMMAND(wxID_ANY, wxEVT_END_THREAD, MainFrame::OnThreadEnd)
    EVT_COMMAND(wxID_ANY, wxEVT_CLOSE_WINDOW, MainFrame::OnFrameClose)
    EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TEXT_ENTER, MainFrame::OnInputFrameNumber)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_SwitchColorYUV, MainFrame::OnDropDownToolbarYUV)
    EVT_SIZE(MainFrame::OnMainFrameSizeChange)
    EVT_IDLE(MainFrame::OnIdle)
    EVT_LISTBOX(wxID_ANY, MainFrame::OnThumbnailLboxSelect)
    EVT_UPDATE_UI_RANGE(ID_ToolBarLowestID, ID_ToolBarHighestID, MainFrame::OnUpdateUI)
    EVT_TIMER(TIMER_ID_PLAYING, MainFrame::OnTimer)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style)
            : wxFrame(parent, id, title, pos, size, style),
            m_bYUVFile(false), m_bOPened(false), m_eYUVComponentChoose(MODE_ORG), m_pcPicYuvOrg(NULL), m_pThumbThread(NULL),
            m_pCenterPageManager(NULL)
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
    m_pTimer = new wxTimer(this, TIMER_ID_PLAYING);
    m_mgr.Update();
}

void MainFrame::CreateFileIOToolBar()
{
    m_ioToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    m_ioToolBar->SetToolBitmapSize(wxSize(16, 16));
    wxBitmap tb_open = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 16));
    m_ioToolBar->AddTool(wxID_OPEN, _T("Open File"), tb_open, _T("Open File"), wxITEM_NORMAL);
    m_ioToolBar->AddTool(ID_ReOpenWrongConfigYUVFile, _T("Reopen current YUV File"), tb_open,
                         _T("Reopen current YUV File if configurations are not right"), wxITEM_NORMAL);
    m_ioToolBar->Realize();

    m_mgr.AddPane(m_ioToolBar, wxAuiPaneInfo().Name(_T("File_IO")).Caption(_T("File IO ToolBar")).
                  ToolbarPane().Top().LeftDockable(false).RightDockable(false));
}

void MainFrame::CreateYUVToolBar()
{
    wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString s;
    wxString str;
    for(int i = 0; i < 10; i++)
    {
        str.Printf(_T("%d"), i);
        s.Add(str);
    }
    tv.SetIncludes(s);

    m_yuvToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                    wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_TEXT | wxAUI_TB_HORZ_TEXT);
    m_pFrameNumberText = new wxTextCtrl(m_yuvToolBar, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(40, -1), wxTE_PROCESS_ENTER, tv);
    m_pTotalFrameNumberText = new wxStaticText(m_yuvToolBar, wxID_ANY, _T("/ 0"), wxDefaultPosition, wxSize(30, -1));
    m_pFrameNumberText->SetToolTip(_T("Current Frame Number"));
    m_pTotalFrameNumberText->SetToolTip(_T("Total Frame Number"));
    m_yuvToolBar->SetToolBitmapSize(wxSize(16, 16));
    wxBitmap tb_switchgrid = wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_TOOLBAR, wxSize(16, 16));
    wxBitmap tb_switchcolor = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_TOOLBAR, wxSize(16, 16));
    wxBitmap tb_back = wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_TOOLBAR, wxSize(16, 16));
    wxBitmap tb_play = wxArtProvider::GetBitmap(wxART_HELP_PAGE, wxART_TOOLBAR, wxSize(16, 16));
    wxBitmap tb_next = wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_TOOLBAR, wxSize(16, 16));
    wxString label[4] = { _T("YUV"), _T("Y"), _T("U"), _T("V") };
    m_yuvToolBar->AddTool(ID_SwitchHEXPixel, _T(""), tb_switchgrid, _T("Switch HEX View in Pixel"), wxITEM_CHECK);
    m_yuvToolBar->AddSeparator();
    m_yuvToolBar->AddTool(ID_SwitchColorYUV, label[m_eYUVComponentChoose], tb_switchcolor, _T("Switch Color YUV"), wxITEM_NORMAL);
    m_yuvToolBar->SetToolDropDown(ID_SwitchColorYUV, true);
    m_yuvToolBar->AddTool(ID_SwitchGrid, _T(""), tb_switchgrid, _T("Switch Grid"), wxITEM_CHECK);
    m_yuvToolBar->AddTool(ID_SwitchfitMode, _T(""), tb_switchgrid, _T("Switch FitMode"), wxITEM_CHECK);
    m_yuvToolBar->AddSeparator();
    m_yuvToolBar->AddTool(ID_GoToPreFrame, _T(""), tb_back, _T("Go to previous frame (PAGE_UP)"), wxITEM_NORMAL);
    m_yuvToolBar->AddTool(ID_FastBackward, _T(""), tb_back, _T("Fast backward"), wxITEM_NORMAL);
    m_yuvToolBar->AddTool(ID_Play_Pause, _T(""), tb_play, _T("Play/Pause"), wxITEM_NORMAL);
    m_yuvToolBar->AddTool(ID_FastForward, _T(""), tb_next, _T("Fast forward"), wxITEM_NORMAL);
    m_yuvToolBar->AddTool(ID_GoToNextFrame, _T(""), tb_next, _T("Go to next frame (PAGE_DOWN)"), wxITEM_NORMAL);
    m_yuvToolBar->AddControl(m_pFrameNumberText, _T("Frame Number"));
    m_yuvToolBar->AddControl(m_pTotalFrameNumberText, _T("Total Frame Number"));
    m_yuvToolBar->Realize();

    m_mgr.AddPane(m_yuvToolBar, wxAuiPaneInfo().Name(_T("YUV_Tools")).Caption(_T("YUV ToolBar")).
                  ToolbarPane().Top().Position(1).LeftDockable(false).RightDockable(false));
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
    wxStatusBar* pStatusBar = GetStatusBar();
    pStatusBar->SetStatusText(_T("Ready"));
    // first is tips, second is msg, the last is the scale ctrl
    pStatusBar->SetFieldsCount(3);
    SetMinSize(wxSize(400, 300));

    CreateFileIOToolBar();
    CreateYUVToolBar();
    g_uiMaxCUWidth  = 64;
    g_uiMaxCUHeight = 64;
}

void MainFrame::CreateNoteBookPane()
{
    m_mgr.AddPane(CreateLeftNotebook(), wxAuiPaneInfo().Name(_T("Left NoteBook")).Caption(_T("YUV info")).
                  BestSize(wxSize(300,100)).MaxSize(wxSize(500,100)).Left().Layer(1));
    m_mgr.AddPane(CreateCenterNotebook(), wxAuiPaneInfo().Name(_T("Center NoteBook")).Center().Layer(0));
    m_mgr.AddPane(CreateBottomNotebook(), wxAuiPaneInfo().Name(_T("Bottom NoteBook")).Bottom().Layer(1));
}

MainFrame::~MainFrame()
{
    ClearThumbnalMemory();
    delete m_pCenterPageManager;
    m_mgr.UnInit();
}

void MainFrame::OnExit(wxCommandEvent& evt)
{
    Close(true);
}

wxNotebook* MainFrame::CreateBottomNotebook()
{
    wxNotebook* ctrl = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize(460,200), 0 );
    wxGridSizer* gSizer = new wxGridSizer(1, 0, 0);
    wxPanel* pLogPanel = new wxPanel(ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
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
    wxPanel* pThumbnalListPanel = new wxPanel(ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    m_pThumbnalList = new wxSimpleHtmlListBox(pThumbnalListPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    gSizer->Add(m_pThumbnalList, 0, wxEXPAND, 5);
    pThumbnalListPanel->SetSizer(gSizer);
    pThumbnalListPanel->Layout();
    ctrl->AddPage(pThumbnalListPanel, _T("Thumbnails"), true);
    // aad ruler for cu pixel
    wxFlexGridSizer* fgSizerUp = new wxFlexGridSizer(2, 1, 0, 0);
    wxFlexGridSizer* fgSizerLeft = new wxFlexGridSizer(1, 2, 0, 0);
    fgSizerUp->AddGrowableCol(0);
    fgSizerUp->AddGrowableRow(1);
    fgSizerUp->SetFlexibleDirection(wxBOTH);
    fgSizerUp->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    fgSizerLeft->AddGrowableCol(1);
    fgSizerLeft->AddGrowableRow(0);
    fgSizerLeft->SetFlexibleDirection(wxBOTH);
    fgSizerLeft->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    wxPanel* pCUPixelPanel = new wxPanel(ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    m_pPixelHRuler = new RulerCtrl(pCUPixelPanel, wxID_ANY);
    m_pPixelVRuler = new RulerCtrl(pCUPixelPanel, wxID_ANY, true);
    fgSizerUp->Add(m_pPixelHRuler, 0, wxEXPAND, 5);
    fgSizerLeft->Add(m_pPixelVRuler, 0, wxEXPAND, 5);

    m_pPixelViewCtrl = new PixelViewCtrl(pCUPixelPanel, wxID_ANY, m_pPixelHRuler, m_pPixelVRuler);
    wxGridSizer* pixelSizer = new wxGridSizer(1, 0, 0);
    pixelSizer->Add(m_pPixelViewCtrl, 0, wxEXPAND, 5 );
    pCUPixelPanel->SetSizer(fgSizerUp);
    pCUPixelPanel->Layout();
    fgSizerLeft->Add(m_pPixelViewCtrl, 1, wxEXPAND, 5);
    fgSizerUp->Add(fgSizerLeft, 1, wxEXPAND, 5);
    ctrl->AddPage(pCUPixelPanel, _T("CU Pixels"), false);
    return ctrl;
}

wxNotebook* MainFrame::CreateCenterNotebook()
{
    wxNotebook* ctrl = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(460,200), 0);
    m_pCenterPageManager = new CenterPageManager(ctrl, m_pThumbnalList, this, m_pPixelViewCtrl);
    m_pCenterPageManager->InsertNewPage(0, _T("Decode Pic"));
    m_pCenterPageManager->Show();
    return ctrl;
}

void MainFrame::OnOpenYUVFile(const wxString& sFile, const wxString& sName, bool bWrongOpened)
{
    int w, h;
    bool bit;
    bool bShowConfigDlg = true;
    if(GetYUVConfigData(sFile, w, h, bit))
        bShowConfigDlg = false;
    if(bWrongOpened)
        bShowConfigDlg = true;
    if(bShowConfigDlg)
    {
        YUVConfigDlg cdlg(this, bWrongOpened);
        wxString width,height;
        if(g_parseResolutionFromFilename(sName, width, height))
        {
            cdlg.SetWidth(width);
            cdlg.SetHeight(height);
        }
        int ret = sName.find(_T("_10bit_"));
        if(ret != wxNOT_FOUND)
            cdlg.SetBitFlag(true);
        if(cdlg.ShowModal() == wxID_CANCEL)
            return;

        m_iSourceWidth = cdlg.GetWidth();
        m_iSourceHeight = cdlg.GetHeight();
        m_iYUVBit = (cdlg.Is10bitYUV() ? 10 : 8);
        m_FileLength = wxFile((const wxChar*)sFile).Length();
        int t = (cdlg.Is10bitYUV() ? 2 : 1);
        if(!(m_FileLength/(m_iSourceWidth*m_iSourceHeight*3/2*t) > 0))
        {
            wxMessageBox(_T("Not enough data for one frame, open YUV file failed"));
            return;
        }
        StoreYUVConfigData(sFile, m_iSourceWidth, m_iSourceHeight, (m_iYUVBit > 8));
    }
    else
    {
        m_iSourceWidth = w;
        m_iSourceHeight = h;
        m_iYUVBit = (bit ? 10 : 8);
        m_FileLength = wxFile((const wxChar*)sFile).Length();
        int t = (bit ? 2 : 1);
        if(!(m_FileLength/(m_iSourceWidth*m_iSourceHeight*3/2*t) > 0))
        {
            wxMessageBox(_T("Not enough data for one frame, open YUV file failed"));
            return;
        }
    }
    // multi-thread
    wxString lastFile = sFile;
    wxCommandEvent event;
    OnCloseFile(event);
    m_bOPened = true;
    m_bPlaying = false;
    m_FileLength = wxFile((const wxChar*)lastFile).Length();
    SetTotalFrameNumber();
    m_pCenterPageManager->GetPicViewCtrl(0)->SetScale(1.0);
    m_pCenterPageManager->GetPicViewCtrl(0)->SetFitMode(true);
    m_cYUVIO.open((char *)lastFile.mb_str(wxConvUTF8).data(), false, m_iYUVBit, m_iYUVBit, m_iYUVBit, m_iYUVBit);
    m_pcPicYuvOrg = new TComPicYuv;
    m_pcPicYuvOrg->create( m_iSourceWidth, m_iSourceHeight, 64, 64, 4 );
    double scaleRate = 165.0/m_iSourceWidth;
    InitThumbnailListView();
    g_LogMessage(_T("Initialize thumbnail finished"));
    m_pImageList = new wxImageList((int)m_iSourceWidth*scaleRate, (int)m_iSourceHeight*scaleRate);
    m_pThumbThread = new ThumbnailThread(this, m_pImageList, m_iSourceWidth, m_iSourceHeight, m_iYUVBit, lastFile);
    if(m_pThumbThread->Create() != wxTHREAD_NO_ERROR)
    {
        g_LogError(_T("Can't create the thread!"));
        delete m_pThumbThread;
        m_pThumbThread = NULL;
    }
    else
    {
        if(m_pThumbThread->Run() != wxTHREAD_NO_ERROR)
        {
            g_LogError(_T("Can't create the thread!"));
            delete m_pThumbThread;
            m_pThumbThread = NULL;
        }
    }
}

void MainFrame::OnOpenFile(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("Open YUV file or HEVC stream file"), _T(""), _T(""),
                     _T("YUV files (*.yuv)|*.yuv"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;
    wxString sfile = dlg.GetPath();
    if(sfile.Lower().EndsWith(_T(".yuv")))
        m_bYUVFile = true;
    else
    {
        m_bYUVFile = false;
        g_LogError(_T("The file to be open must be YUV file"));
    }

    if(m_bYUVFile)
    {
        m_sCurOpenedFilePath = dlg.GetPath();
        m_sCurOpenedFileName = dlg.GetFilename();
        OnOpenYUVFile(dlg.GetPath(), dlg.GetFilename());
    }
    else
    {
        OnCloseFile(event);
        m_bOPened = true;
    }
}

void MainFrame::OnFrameClose(wxCommandEvent& event)
{
    OnCloseFile(event);
    event.Skip();
}

void MainFrame::OnCloseFile(wxCommandEvent& event)
{
    if(m_bOPened)
    {
        m_sCurOpenedFilePath = _T("");
        m_sCurOpenedFileName = _T("");
        if(m_pTimer->IsRunning())
            m_pTimer->Stop();
        m_pCenterPageManager->Close();
        if(m_bYUVFile)
        {
            m_cYUVIO.close();
            if(m_pThumbThread)
            {
                if(m_pThumbThread->IsAlive())
                    m_pThumbThread->Delete();
                m_pThumbThread = NULL;
            }
            if(m_pcPicYuvOrg)
            {
                m_pcPicYuvOrg->destroy();
                delete m_pcPicYuvOrg;
                m_pcPicYuvOrg = NULL;
            }
            if(m_StrMemFileName.GetCount())
                ClearThumbnalMemory();
            m_FileLength = 0;
            m_pCenterPageManager->Clear();
            m_pPixelViewCtrl->Clear();
            m_pFrameNumberText->SetValue(_T("0"));
            m_pTotalFrameNumberText->SetLabel(_T("/ 0"));
            g_LogMessage(wxString::Format(_T("OnCloseFile() %d"), m_StrMemFileName.GetCount()));
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
    g_LogMessage(wxString::Format(_T("Add some images from %d to %d"), frame-framenumber+1, frame));
    for(int i = 0;  i < (int)framenumber; i++)
    {
        int tmp = frame-framenumber+i+1;
        if(m_pImageList->GetImageCount() > tmp)
        {
            wxMemoryFSHandler::AddFile(wxString::Format(_T("Frame Number %d.bmp"), tmp), m_pImageList->GetBitmap(tmp),wxBITMAP_TYPE_BMP);
            wxString label = wxString::Format(_T("<span>&nbsp;</span><p align=\"center\"><img src=\"memory:Frame Number %d.bmp\"><br></p><span text-align=center>Frame Number: %d </span><br>"), tmp, tmp);
            m_pThumbnalList->SetString(tmp, label);
            wxString filename = wxString::Format(_T("Frame Number %d.bmp"), tmp);
            m_StrMemFileName.Add(filename);
        }

    }
    m_pThumbnalList->Freeze();
    unsigned int cnt = m_pThumbnalList->GetFirstVisibleLine();
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
    g_LogMessage(wxString::Format(_T("LEAVE Adding some images from %d to %d"), frame-framenumber+1, frame));
}

void MainFrame::OnThreadEnd(wxCommandEvent& event)
{
    g_LogMessage(_T("OnThreadEnd called"));
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
    if(m_pImageList)
    {
        m_pImageList->RemoveAll();
        delete m_pImageList;
        m_pImageList = NULL;
    }
}

void MainFrame::OnThumbnailLboxSelect(wxCommandEvent& event)
{
    wxBusyCursor wait;
    int frame = event.GetInt();
    m_cYUVIO.reset();
    m_cYUVIO.skipFrames(frame, m_iSourceWidth, m_iSourceHeight);
    int pad[] = {0, 0};
    m_cYUVIO.read(m_pcPicYuvOrg, pad);
    m_pCenterPageManager->GetPicViewCtrl(0)->SetPicYuvBuffer(m_pcPicYuvOrg, m_iSourceWidth, m_iSourceHeight, m_iYUVBit);
    wxString str;
    str.Printf(_T("%d"), frame + 1);
    m_pFrameNumberText->SetValue(str);
}

void MainFrame::InitThumbnailListView()
{
    int framenumber = m_FileLength/(m_iSourceWidth*m_iSourceHeight*1.5*(m_iYUVBit > 8? 2 : 1));
    g_LogMessage(wxString::Format(_T("Frame Number: %d\n"), framenumber));
    wxArrayString arr;
    for(int i = 0;  i < framenumber; i++)
    {
        wxString label = wxString::Format(_T("<span>&nbsp;</span><p align=\"center\"><img src=\"/home/luqingbo/Downloads/horse.bmp\"><br></p><span text-align=center>Frame Number: %d </span><br>"), i);
        arr.Add(label);
    }
    m_pThumbnalList->Append(arr);
}

void MainFrame::OnMainFrameSizeChange(wxSizeEvent& event)
{
    if(!m_pCenterPageManager) return;
    if(m_pCenterPageManager->GetPicViewCtrl(0))
    {
        m_pCenterPageManager->GetPicViewCtrl(0)->CalFitScaleRate();
        m_pCenterPageManager->GetPicViewCtrl(0)->SetRulerCtrlFited();
    }
}

void MainFrame::OnIdle(wxIdleEvent& event)
{
    if(!m_pCenterPageManager) return;
    if(m_pCenterPageManager->GetPicViewCtrl(0))
    {
        m_pCenterPageManager->GetPicViewCtrl(0)->CalFitScaleRate();
        m_pCenterPageManager->GetPicViewCtrl(0)->SetRulerCtrlFited();
        event.RequestMore(false);
    }
}

void MainFrame::OnDropDownToolbarYUV(wxAuiToolBarEvent& event)
{
    wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(event.GetEventObject());
    tb->SetToolSticky(event.GetId(), true);
    wxMenu menuPopup;
    menuPopup.AppendRadioItem(ID_Switch_YUV, _T("YUV"), _T("Show YUV"));
    menuPopup.AppendRadioItem(ID_Switch_Y, _T("Y"), _T("Show Y component"));
    menuPopup.AppendRadioItem(ID_Switch_U, _T("U"), _T("Show U component"));
    menuPopup.AppendRadioItem(ID_Switch_V, _T("V"), _T("Show V component"));
    menuPopup.Check(ID_Switch_YUV + (int)m_eYUVComponentChoose, true);
    wxRect rect = tb->GetToolRect(event.GetId());
    wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
    pt = ScreenToClient(pt);
    PopupMenu(&menuPopup, pt);
    tb->SetToolSticky(event.GetId(), false);
}

void MainFrame::OnSwitchShowGrid(wxCommandEvent& event)
{
    bool b = event.IsChecked();
    unsigned int size = m_pCenterPageManager->GetSize();
    for(unsigned int i = 0; i < size; i++)
    {
        PicViewCtrl* pCtrl = m_pCenterPageManager->GetPicViewCtrl(i);
        pCtrl->SetShowGrid(b);
        pCtrl->Refresh();
    }
}

void MainFrame::SetColorComponent()
{
    unsigned int size = m_pCenterPageManager->GetSize();
    for(unsigned int i = 0; i < size; i++)
    {
        PicViewCtrl* pCtrl = m_pCenterPageManager->GetPicViewCtrl(i);
        pCtrl->SetWhichTobeShown(m_eYUVComponentChoose);
        pCtrl->Refresh();
    }
}

void MainFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
    PicViewCtrl* pCtrl = m_pCenterPageManager->GetPicViewCtrl(0);
    switch(event.GetId())
    {
    case ID_ReOpenWrongConfigYUVFile:
    case ID_GoToNextFrame:
    case ID_GoToPreFrame:
    case ID_Play_Pause:
    case ID_FastForward:
    case ID_FastBackward:
        event.Enable(m_bOPened);
        break;
    case ID_SwitchGrid:
        event.Check(pCtrl->IsShowGrid());
        break;
    case ID_Switch_YUV:
        event.Check(m_eYUVComponentChoose == MODE_ORG);
        break;
    case ID_Switch_Y:
        event.Check(m_eYUVComponentChoose == MODE_Y);
        break;
    case ID_Switch_U:
        event.Check(m_eYUVComponentChoose == MODE_U);
        break;
    case ID_Switch_V:
        event.Check(m_eYUVComponentChoose == MODE_V);
        break;
    case ID_SwitchfitMode:
        event.Check(pCtrl->GetFitMode());
        break;
    case ID_SwitchHEXPixel:
        event.Check(m_pPixelViewCtrl->GetHEXFormat());
        break;
    }
}

void MainFrame::OnSwitchYUV(wxCommandEvent& event)
{
    wxString label[4] = { _T("YUV"), _T("Y"), _T("U"), _T("V") };
    switch(event.GetId())
    {
    case ID_Switch_YUV:
        m_eYUVComponentChoose = MODE_ORG;
        break;
    case ID_Switch_Y:
        m_eYUVComponentChoose = MODE_Y;
        break;
    case ID_Switch_U:
        m_eYUVComponentChoose = MODE_U;
        break;
    case ID_Switch_V:
        m_eYUVComponentChoose = MODE_V;
        break;
    }
    m_yuvToolBar->SetToolLabel(ID_SwitchColorYUV, label[m_eYUVComponentChoose]);
    SetColorComponent();
}

void MainFrame::OnSwitchFitMode(wxCommandEvent& event)
{
    unsigned int size = m_pCenterPageManager->GetSize();
    for(unsigned int i = 0; i < size; i++)
    {
        PicViewCtrl* pCtrl = m_pCenterPageManager->GetPicViewCtrl(i);
        bool bFit = pCtrl->GetFitMode();
        pCtrl->SetFitMode(!bFit);
        if(!bFit)
            pCtrl->Refresh();
    }
}

void MainFrame::OnSwitchHEXPixel(wxCommandEvent& event)
{
    bool bHEX = m_pPixelViewCtrl->GetHEXFormat();
    m_pPixelViewCtrl->SetHEXFormat(!bHEX);
    m_pPixelViewCtrl->Refresh();
}

wxString MainFrame::GetDataBaseFileName(const DataBaseType type)
{
    wxString name = wxStandardPaths::Get().GetUserLocalDataDir();
    if(!::wxDirExists(name))
        ::wxMkdir(name);
    switch(type)
    {
    case ID_SettingData:
        name += _T("/Settings.db");
        break;
    case ID_StreamInfoData:
        name += _T("/Info.db");
        break;
    default:
        assert(0);
        break;
    }
    return name;
}

void MainFrame::StoreYUVConfigData(const wxString& file, int width, int height, bool b10bit)
{
    wxString dbName = GetDataBaseFileName(ID_SettingData);
    wxSQLite3Database* db = new wxSQLite3Database();
    db->Open(dbName);
    assert(db->TableExists(_T("YUVCONFIG")));
    // check file already in the database
    wxString sqlQuery = _T("SELECT * FROM YUVCONFIG WHERE FileName=\"");
    sqlQuery += ( file + _T("\"") );
    wxSQLite3ResultSet result = db->ExecuteQuery(sqlQuery);
    if(result.NextRow())
    {
        // fix with the new data
        wxString sqlUpdate = _T("UPDATE YUVCONFIG SET ");
        sqlUpdate += wxString::Format(_T("Width=\"%d\", "), width);
        sqlUpdate += wxString::Format(_T("Height=\"%d\", "), height);
        sqlUpdate += _T("Is10Bit=");
        wxString bit = b10bit ? _T("\"1\"") : _T("\"0\"");
        sqlUpdate += bit;
        sqlUpdate += ( _T(" WHERE FileName=\"") + file + _T("\"") );
        db->ExecuteUpdate(sqlUpdate);
    }
    else
    {
        wxString sqlInsert = _T("INSERT INTO YUVCONFIG VALUES (");
        sqlInsert += ( _T("\"") + file + _T("\", ") );
        sqlInsert += wxString::Format(_T("\"%d\", "), width);
        sqlInsert += wxString::Format(_T("\"%d\", "), height);
        wxString bit = b10bit ? _T("\"1\"") : _T("\"0\"");
        sqlInsert += ( bit + _T(")") );
        db->ExecuteUpdate(sqlInsert);
    }
    db->Close();
    delete db;
}

bool MainFrame::GetYUVConfigData(const wxString& file, int& width, int& height, bool& b10bit)
{
    wxString dbName = GetDataBaseFileName(ID_SettingData);
    wxSQLite3Database* db = new wxSQLite3Database();
    db->Open(dbName);
    if(!db->TableExists(_T("YUVCONFIG")))
    {
        db->ExecuteUpdate(_T("CREATE TABLE YUVCONFIG (FileName varchar(600), Width int, Height int, Is10Bit bit, PRIMARY KEY (FileName))"));
        return false;
    }
    wxString sqlQuery = _T("SELECT * FROM YUVCONFIG WHERE FileName=\"");
    sqlQuery += ( file + _T("\"") );
    wxSQLite3ResultSet result = db->ExecuteQuery(sqlQuery);
    bool ret = false;
    if(result.NextRow())
    {
        width = result.GetInt(1);
        height = result.GetInt(2);
        b10bit = result.GetBool(3);
        ret = true;
    }
    db->Close();
    delete db;
    return ret;
}

void MainFrame::SetTotalFrameNumber()
{
    int t = m_iYUVBit > 8 ? 2 : 1;
    int framenum = m_FileLength/(m_iSourceWidth*m_iSourceHeight*3/2*t);
    m_iTotalFrame = framenum;
    wxString str;
    str.Printf(_T("/ %d"), framenum);
    m_pTotalFrameNumberText->SetLabel(str);
    m_pFrameNumberText->SetValue(_T("1"));
}

void MainFrame::OnGoToNextFrame(wxCommandEvent& event)
{
    if(m_bOPened && !m_bPlaying && m_pCenterPageManager->GetPicViewCtrl(0))
        m_pCenterPageManager->GetPicViewCtrl(0)->ShowPageByDiffNumber(1);
}

void MainFrame::OnGoToPreFrame(wxCommandEvent& event)
{
    if(m_bOPened && !m_bPlaying && m_pCenterPageManager->GetPicViewCtrl(0))
        m_pCenterPageManager->GetPicViewCtrl(0)->ShowPageByDiffNumber(-1);
}

void MainFrame::OnPlayorPause(wxCommandEvent& event)
{
    if(m_bOPened && m_pCenterPageManager->GetPicViewCtrl(0))
    {
        if(m_pTimer->IsRunning())
        {
            m_bPlaying = false;
            m_pTimer->Stop();
        }
        else
        {
            m_bPlaying = true;
            m_pTimer->Start(100/3);
        }
    }
}

void MainFrame::OnFastForward(wxCommandEvent& event)
{
    if(m_bOPened && !m_bPlaying && m_pCenterPageManager->GetPicViewCtrl(0))
        m_pCenterPageManager->GetPicViewCtrl(0)->ShowPageByDiffNumber(10);
}

void MainFrame::OnFastBackward(wxCommandEvent& event)
{
    if(m_bOPened && !m_bPlaying && m_pCenterPageManager->GetPicViewCtrl(0))
        m_pCenterPageManager->GetPicViewCtrl(0)->ShowPageByDiffNumber(-10);
}

void MainFrame::OnTimer(wxTimerEvent& event)
{
    int frame = m_pThumbnalList->GetSelection();
    if(frame == m_iTotalFrame - 1)
    {
        m_pTimer->Stop();
        m_bPlaying = false;
    }
    else
        m_pCenterPageManager->GetPicViewCtrl(0)->ShowPageByDiffNumber(1, true);
}

void MainFrame::OnInputFrameNumber(wxCommandEvent& event)
{
    if(m_bOPened && !m_bPlaying)
    {
        if(event.GetEventObject() == (wxObject*)m_pFrameNumberText)
        {
            int frame = m_pThumbnalList->GetSelection();
            wxString str = m_pFrameNumberText->GetValue();
            long diff = 0;
            str.ToLong(&diff);
            diff = diff - frame - 1;
            m_pCenterPageManager->GetPicViewCtrl(0)->ShowPageByDiffNumber(diff);
        }
    }
    else
        m_pFrameNumberText->SetValue(_T("0"));
}

void MainFrame::OnReOpenWrongConfigYUVFile(wxCommandEvent& event)
{
    if(m_bOPened)
        OnOpenYUVFile(m_sCurOpenedFilePath, m_sCurOpenedFileName, true);
}

CenterPageManager::~CenterPageManager()
{
    Destory();
}

void CenterPageManager::Destory()
{
    m_pCenterNoteBook->DeleteAllPages();
    m_PageList.clear();
}

void CenterPageManager::Close()
{
    std::size_t size = m_PageList.size();
    assert(size >= 1);
    for(std::size_t i = 1; i < size; i++)
        m_pCenterNoteBook->DeletePage(i);
    m_pCenterNoteBook->SetPageText(0, _T("Decode Pic"));
    m_PageList.begin()->_name = _T("Decode Pic");
    std::list<PanelElments>::iterator it = m_PageList.begin();
    it++;
    m_PageList.erase(it, m_PageList.end());
}

void CenterPageManager::InsertNewPage(const int insertAt, const wxString& name)
{
    wxPanel* pDecodePanel = new wxPanel(m_pCenterNoteBook, wxID_ANY, wxDefaultPosition,
                                wxDefaultSize, wxTAB_TRAVERSAL);
    wxScrolledWindow* pDecodeScrolledWin = new wxScrolledWindow(pDecodePanel, -1,
                                wxDefaultPosition, wxDefaultSize, wxScrolledWindowStyle);
    RulerCtrl* pPicHRuler = new RulerCtrl(pDecodePanel, wxID_ANY);
    RulerCtrl* pPicVRuler = new RulerCtrl(pDecodePanel, wxID_ANY, true);
    PicViewCtrl* pPicViewCtrl = new PicViewCtrl(pDecodeScrolledWin, wxID_ANY, m_pList, pPicHRuler,
                                pPicVRuler, m_pPixelViewCtrl, m_pMainFrame);
    PanelElments elments;
    elments._pPicPanel          = pDecodePanel;
    elments._pDecodeScrolledWin = pDecodeScrolledWin;
    elments._pPicViewCtrl       = pPicViewCtrl;
    elments._pPicHRuler         = pPicHRuler;
    elments._pPicVRuler         = pPicVRuler;
    elments._name               = name;
    std::list<PanelElments>::iterator it = m_PageList.begin();
    for(int i = 0; i < insertAt; i++)
        it++;
    m_PageList.insert(it, elments);
}

void CenterPageManager::Show()
{
    std::list<PanelElments>::iterator it = m_PageList.begin();
    if(m_bFirstCreate)
        m_bFirstCreate = false;
    else
        it++;
    for(; it != m_PageList.end(); it++)
    {
        PanelElments elments = *it;
        wxFlexGridSizer* fgSizerUp = new wxFlexGridSizer(2, 1, 0, 0);
        wxFlexGridSizer* fgSizerLeft = new wxFlexGridSizer(1, 2, 0, 0);
        fgSizerUp->AddGrowableCol(0);
        fgSizerUp->AddGrowableRow(1);
        fgSizerUp->SetFlexibleDirection(wxBOTH);
        fgSizerUp->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizerLeft->AddGrowableCol(1);
        fgSizerLeft->AddGrowableRow(0);
        fgSizerLeft->SetFlexibleDirection(wxBOTH);
        fgSizerLeft->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizerUp->Add(elments._pPicHRuler, 0, wxEXPAND, 5);
        fgSizerLeft->Add(elments._pPicVRuler, 0, wxEXPAND, 5);
        elments._pPicViewCtrl->SetSizeHints(300, 300);
        wxGridSizer* innerSizer = new wxGridSizer(1, 0, 0);
        innerSizer->Add(elments._pPicViewCtrl, 1, wxALIGN_CENTER);
        elments._pDecodeScrolledWin->SetScrollRate(4, 4);
        elments._pDecodeScrolledWin->SetSizer(innerSizer);
        fgSizerLeft->Add(elments._pDecodeScrolledWin, 1, wxEXPAND, 5);
        fgSizerUp->Add(fgSizerLeft, 1, wxEXPAND, 5);
        elments._pPicPanel->SetSizer(fgSizerUp);
        elments._pPicPanel->Layout();
        bool bFirst = (it == m_PageList.begin());
        m_pCenterNoteBook->AddPage(elments._pPicPanel, elments._name, bFirst);
    }
}

PicViewCtrl* CenterPageManager::GetPicViewCtrl(const std::size_t index)
{
    if(index >= m_PageList.size())
        return NULL;
    std::list<PanelElments>::iterator it = m_PageList.begin();
    for(std::size_t i = 0; i < index; i++)
        it++;
    return (*it)._pPicViewCtrl;
}

wxScrolledWindow* CenterPageManager::GetDecodeScrolledWin(const std::size_t index)
{
    if(index >= m_PageList.size())
        return NULL;
    std::list<PanelElments>::iterator it = m_PageList.begin();
    for(std::size_t i = 0; i < index; i++)
        it++;
    return (*it)._pDecodeScrolledWin;
}

RulerCtrl* CenterPageManager::GetHorRulerCtrl(const std::size_t index)
{
    if(index >= m_PageList.size())
        return NULL;
    std::list<PanelElments>::iterator it = m_PageList.begin();
    for(std::size_t i = 0; i < index; i++)
        it++;
    return (*it)._pPicHRuler;
}

RulerCtrl* CenterPageManager::GetVerRulerCtrl(const std::size_t index)
{
    if(index >= m_PageList.size())
        return NULL;
    std::list<PanelElments>::iterator it = m_PageList.begin();
    for(std::size_t i = 0; i < index; i++)
        it++;
    return (*it)._pPicVRuler;
}

wxString CenterPageManager::GetName(const std::size_t index)
{
    if(index >= m_PageList.size())
        return _T("");
    std::list<PanelElments>::iterator it = m_PageList.begin();
    for(std::size_t i = 0; i < index; i++)
        it++;
    return (*it)._name;
}

void CenterPageManager::Clear()
{
    std::size_t size = m_PageList.size();
    for(std::size_t i = 0; i < size; i++)
    {
        PicViewCtrl* pPic = GetPicViewCtrl(i);
        pPic->Clear();
    }
}

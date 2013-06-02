#include "MainFrame.h"
#include "YUVConfigDlg.h"
#include <wx/fs_mem.h>

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
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style)
            : wxFrame(parent, id, title, pos, size, style),
            m_bYUVFile(false), m_bOPened(false), m_pThumbThread(NULL) 
{
    //SetSizeHints( wxDefaultSize, wxDefaultSize );
    //m_StrMemFileName = new wxArrayString();
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

    m_mgr.Update();
}

void MainFrame::CreateMenuToolBar()
{
    wxMenuBar* mb = new wxMenuBar;
    wxMenu* file_menu = new wxMenu;
    file_menu->Append(wxID_OPEN, wxT("Open File"));
    file_menu->Append(wxID_CLOSE, wxT("Close Current File"));
    file_menu->Append(wxID_EXIT, wxT("Exit"));
    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, wxT("About HEVC Analyser"));

    mb->Append(file_menu, wxT("File"));
    mb->Append(help_menu, wxT("Help"));

    SetMenuBar(mb);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(wxT("Ready"));
    SetMinSize(wxSize(400, 300));

    wxAuiToolBar* tb = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    tb->SetToolBitmapSize(wxSize(16, 16));
    wxBitmap tb_bmp1 = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16, 16));
    tb->AddTool(wxID_OPEN, wxT("Open File"), tb_bmp1, wxT("Open File"), wxITEM_NORMAL);
    tb->AddTool(ID_CLOSE_FILE, wxT("CLose File"), tb_bmp1, wxT("Close File"), wxITEM_NORMAL);
    tb->Realize();

    m_mgr.AddPane(tb, wxAuiPaneInfo().
                  Name(wxT("tb")).Caption(wxT("Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));
    g_uiMaxCUWidth  = 64;
    g_uiMaxCUHeight = 64;
}

void MainFrame::CreateNoteBookPane()
{
    m_pLeftNoteBook = CreateLeftNotebook();
    m_mgr.AddPane(m_pLeftNoteBook, wxAuiPaneInfo().Name(wxT("Left NoteBook")).
                                                        Caption(wxT("YUV info")).
                  BestSize(wxSize(300,100)).MaxSize(wxSize(500,100)).Left().Layer(1));
    m_mgr.AddPane(new wxTextCtrl(this,wxID_ANY, _("test"),
                          wxPoint(0,0), wxSize(150,90),
                          wxNO_BORDER | wxTE_MULTILINE), wxAuiPaneInfo().Name(wxT("Center NoteBook")).Center().Layer(0));
    m_mgr.AddPane(new wxTextCtrl(this,wxID_ANY, _("test"),
                          wxPoint(0,0), wxSize(150,90),
                          wxNO_BORDER | wxTE_MULTILINE), wxAuiPaneInfo().Name(wxT("Bottom NoteBook")).Bottom().Layer(1));
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

wxNotebook* MainFrame::CreateLeftNotebook()
{
    wxSize client_size = GetClientSize();
    wxNotebook* ctrl = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize(460,200), 0 );
    //wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
    wxGridSizer* gSizer;
    gSizer = new wxGridSizer( 1, 0, 0 );
    m_pThumbnalListPanel = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
//  m_pThumbnalList = new wxListCtrl( m_pThumbnalListPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize , wxNO_BORDER | wxLC_ICON);
    m_pThumbnalList = new wxSimpleHtmlListBox(m_pThumbnalListPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
                                              0, NULL, 0);
//    wxArrayString arr;
//    wxString label = wxString::Format(_T("<h>poc%d</h>"), 100);
//    arr.Add(label);
//    m_pThumbnalList->Append(arr);
    gSizer->Add( m_pThumbnalList, 0, wxEXPAND, 5 );

    m_pThumbnalListPanel->SetSizer( gSizer );
    m_pThumbnalListPanel->Layout();
//    gSizer->Fit( m_pThumbnalListPanel );
//    m_pThumbnalList->Fit( m_pThumbnalListPanel );
    ctrl->AddPage( m_pThumbnalListPanel, wxT("Thumbnails"), true );
    wxPanel* m_panel7 = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    ctrl->AddPage( m_panel7, wxT("CU Pixels"), false );
//    ctrl->AddPage( new wxPanel( ctrl) , wxT("wxTextCtrl 1"), false );
//    ctrl->AddPage( new wxPanel( ctrl) , wxT("wxTextCtrl 1"), false );

    return ctrl;
}

void MainFrame::OnOpenFile(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("Open YUV file or HEVC stream file"), wxT(""), wxT(""),
                     wxT("YUV files (*.yuv)|*.yuv"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;
    wxString sfile = dlg.GetPath();
    if(sfile.EndsWith(wxT(".yuv")))
        m_bYUVFile = true;
    else
        m_bYUVFile = false;

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
        int bit = (cdlg.Is10bitYUV() ? 10 : 8);
        double scaleRate = 165.0/m_iSourceWidth;
        m_pImageList = new wxImageList((int)m_iSourceWidth*scaleRate, (int)m_iSourceHeight*scaleRate);
//        m_pThumbnalList->SetImageList(m_pImageList, wxIMAGE_LIST_NORMAL);
        m_pThumbThread = new ThumbnailThread(this, m_pImageList, m_iSourceWidth, m_iSourceHeight, bit, sfile);
        if(m_pThumbThread->Create() != wxTHREAD_NO_ERROR)
        {
            wxLogError(wxT("Can't create the thread!"));
            delete m_pThumbThread;
            m_pThumbThread = NULL;
        }
        else
        {
            if(m_pThumbThread->Run() != wxTHREAD_NO_ERROR)
            {
                wxLogError(wxT("Can't create the thread!"));
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
            //m_pThumbnalList->ClearAll();
            if(m_StrMemFileName.GetCount())
                ClearThumbnalMemory();
        }
        else
        {

        }
        m_bOPened = false;
    }
}

void MainFrame::AddThumbnailListSingleThread()
{
    TComPicYuv* pcPicYuvOrg = new TComPicYuv;
    pcPicYuvOrg->create( m_iSourceWidth, m_iSourceHeight, 64, 64, 4 );
    m_pImageList = new wxImageList((int)m_iSourceWidth*0.2, (int)m_iSourceHeight*0.2);
    //m_pThumbnalList->SetImageList(m_pImageList, wxIMAGE_LIST_NORMAL);
    wxBitmap bmp(m_iSourceWidth, m_iSourceHeight, 24);
    int frame = 0;
    while(!m_cYUVIO.isEof())
    {
        int pad[] = {0, 0};
        m_cYUVIO.read(pcPicYuvOrg, pad);
        wxNativePixelData img(bmp);
        wxNativePixelData::Iterator p(img);
        for(int j = 0; j < m_iSourceHeight; j++)
        {
            wxNativePixelData::Iterator rowStart = p;
            Pel* pY = pcPicYuvOrg->getLumaAddr() + j*pcPicYuvOrg->getStride();
            Pel* pU = pcPicYuvOrg->getCbAddr()   + (j/2)*pcPicYuvOrg->getCStride();
            Pel* pV = pcPicYuvOrg->getCrAddr()   + (j/2)*pcPicYuvOrg->getCStride();
            for(int i = 0; i < m_iSourceWidth; i++)
            { 
                Pel y = pY[i];
                Pel u = pU[i/2];
                Pel v = pV[i/2];
                int r = y + 1.402*(v-128);
                int g = y - 0.344*(u-128) - 0.714*(v-128);
                int b = y + 1.722*(u-128);
                r = r>255? 255 : r<0 ? 0 : r;
                g = g>255? 255 : g<0 ? 0 : g;
                b = b>255? 255 : b<0 ? 0 : b;
                p.Red() = r;
                p.Green() = g;
                p.Blue() = b;
                p++;
            }
            p = rowStart;
            p.OffsetY(img, 1);
        }
        //bmp.SaveFile(_("test.bmp"), wxBITMAP_TYPE_BMP);
        wxImage bimg = bmp.ConvertToImage();
        wxImage simg = bimg.Scale((int)m_iSourceWidth*0.2, (int)m_iSourceHeight*0.2);
        wxBitmap newbmp(simg);
        m_pImageList->Add(newbmp);
        wxListItem item;
        item.SetId(frame);
        wxString text;
        text.Printf(wxT("POC %d"), frame);
        item.SetText(text);
        item.SetImage(frame);
        //m_pThumbnalList->InsertItem(item);
        frame++;
    }

    pcPicYuvOrg->destroy();
    delete pcPicYuvOrg;
    pcPicYuvOrg = NULL;
}

void MainFrame::OnThreadAddImage(wxCommandEvent& event)
{
   int frame = event.GetInt();
   wxString filename = wxString::Format(_T("poc %d.bmp"), frame);
   m_StrMemFileName.Add(filename);
   wxArrayString arr;
   wxMemoryFSHandler::AddFile(wxString::Format(_T("poc %d.bmp"), frame), m_pImageList->GetBitmap(frame),wxBITMAP_TYPE_BMP);
   wxString label = wxString::Format(_T("<p align=\"center\"><img src=\"memory:poc %d.bmp\"><br></p><span text-align=center>poc%d </span><br><span>&nbsp;</span>"), frame, frame);
   arr.Add(label);
   m_pThumbnalList->Append(arr);
   m_pThumbnalList->RefreshAll();

   m_mgr.Update();
}

void MainFrame::OnThreadEnd(wxCommandEvent& event)
{
    m_pThumbThread = NULL;
}

void MainFrame::ClearThumbnalMemory()
{
//    wxLogError(wxString::Format(_T("clearing: %d"), m_StrMemFileName.GetCount()));
    for(int i = 0; i < m_StrMemFileName.GetCount(); i++)
        wxMemoryFSHandler::RemoveFile(m_StrMemFileName[i]);
    m_StrMemFileName.Clear();
}

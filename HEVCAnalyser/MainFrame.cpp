#include "MainFrame.h"

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

IMPLEMENT_APP(HEVCodecApp)

bool HEVCodecApp::OnInit()
{
    wxFrame* frame = new MainFrame(NULL, wxID_ANY, wxT("HEVC Codec Stream Analyser"),
                                 wxDefaultPosition, wxSize(800, 600));
    SetTopWindow(frame);
    frame->Show();
    return true;
}

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpenFile)
    EVT_MENU(wxID_CLOSE, MainFrame::OnCloseFile)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style)
            : wxFrame(parent, id, title, pos, size, style),
            m_bYUVFile(false), m_bOPened(false)
{
    //SetSizeHints( wxDefaultSize, wxDefaultSize );
    Centre();
    m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
    m_mgr.SetManagedWindow(this);
    // this is not define yet
    // SetIcon(wxIcon(sample_xpm));
    m_notebook_style = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS
                    | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
    m_notebook_theme = 1;

    CreateMenuToolBar();
    CreateNoteBookPane();

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
    gSizer = new wxGridSizer( 0, 1, 0, 0 );
    m_pThumbnalListPanel = new wxPanel( ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_pThumbnalList = new wxListCtrl( m_pThumbnalListPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize , wxNO_BORDER | wxLC_ICON);
    gSizer->Add( m_pThumbnalList, 0, wxALL|wxEXPAND, 5 );

    m_pThumbnalListPanel->SetSizer( gSizer );
    m_pThumbnalListPanel->Layout();
    gSizer->Fit( m_pThumbnalListPanel );
    //m_pThumbnalList->Fit( m_pThumbnalListPanel );
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
    {
        m_bOPened = false;
        return;
    }
    wxString sfile = dlg.GetPath();
    if(sfile.EndsWith(wxT(".yuv")))
        m_bYUVFile = true;
    else
        m_bYUVFile = false;
    OnCloseFile(event);
    m_bOPened = true;
    if(m_bYUVFile)
    {
        m_cYUVIO.open((char *)sfile.mb_str(wxConvUTF8).data(), false, 8, 8, 8, 8);

        m_iSourceWidth = 832;
        m_iSourceHeight = 480;
        AddThumbnailList();
    }
    else
    {

    }
}

void MainFrame::OnCloseFile(wxCommandEvent& event)
{
    if(m_bOPened)
    {
        if(m_bYUVFile)
        {
            m_cYUVIO.close();
            m_pThumbnalList->ClearAll();
        }
        else
        {

        }
        m_bOPened = false;
    }
}

void MainFrame::AddThumbnailList()
{
    TComPicYuv* pcPicYuvOrg = new TComPicYuv;
    pcPicYuvOrg->create( m_iSourceWidth, m_iSourceHeight, 64, 64, 4 );
    wxImageList* pImage_list = new wxImageList((int)m_iSourceWidth*0.2, (int)m_iSourceHeight*0.2);
    m_pThumbnalList->SetImageList(pImage_list, wxIMAGE_LIST_NORMAL);
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
        pImage_list->Add(newbmp);
        wxListItem item;
        item.SetId(frame);
        wxString text;
        text.Printf(wxT("POC %d"), frame);
        item.SetText(text);
        item.SetImage(frame);
        m_pThumbnalList->InsertItem(item);
        frame++;
    }

    pcPicYuvOrg->destroy();
    delete pcPicYuvOrg;
    pcPicYuvOrg = NULL;
}

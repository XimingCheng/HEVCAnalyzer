#include "YUVConfigDlg.h"

BEGIN_EVENT_TABLE(YUVConfigDlg, wxDialog)
    EVT_BUTTON(ID_OKBUTTON, YUVConfigDlg::OnOK)
    EVT_BUTTON(ID_CANCELBUTTON, YUVConfigDlg::OnCancel)
END_EVENT_TABLE()

YUVConfigDlg::YUVConfigDlg(wxWindow *parent, bool bReload)
    : wxDialog(parent, wxID_ANY, wxString(_T("YUV Param Settings"))),
    m_iWith(-1), m_iHeight(-1), m_bReload(bReload)
{
    SetSize(210, 160);
    wxFlexGridSizer* fgSizer;
    fgSizer = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer->SetFlexibleDirection( wxBOTH );
    fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString s;
    wxString str;
    for(int i = 0; i < 10; i++)
    {
        str.Printf(_T("%d"), i);
        s.Add(str);
    }
    tv.SetIncludes(s);

    wxStaticText* staticTextW = new wxStaticText( this, wxID_ANY, _T("YUV Width"), wxDefaultPosition, wxDefaultSize, 0 );
    staticTextW->Wrap( -1 );
    fgSizer->Add( staticTextW, 0, wxALIGN_CENTER|wxALL, 5 );

    m_textCtrlW = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, tv );
    fgSizer->Add( m_textCtrlW, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText* staticTextH = new wxStaticText( this, wxID_ANY, _T("YUV Height"), wxDefaultPosition, wxDefaultSize, 0 );
    staticTextH->Wrap( -1 );
    fgSizer->Add( staticTextH, 0, wxALIGN_CENTER|wxALL, 5 );

    m_textCtrlH = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, tv );
    fgSizer->Add( m_textCtrlH, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText* staticTextBit = new wxStaticText( this, wxID_ANY, _T("YUV Bits"), wxDefaultPosition, wxDefaultSize, 0 );
    staticTextBit->Wrap( -1 );
    fgSizer->Add( staticTextBit, 0, wxALIGN_CENTER|wxALL, 5 );

    wxArrayString choiceBitStr;
    choiceBitStr.Add(wxT("8-Bit  YUV420"));
    choiceBitStr.Add(wxT("10-Bit YUV420"));
    m_choiceBit = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choiceBitStr, 0 );
    m_choiceBit->SetSelection( 0 );
    fgSizer->Add( m_choiceBit, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton* buttonCancel = new wxButton( this, ID_CANCELBUTTON, _T("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer->Add( buttonCancel, 0, wxALL, 5 );

    wxString OKtext = _T("OK");
    if(m_bReload) OKtext = _T("Reload");
    wxButton* buttonOK = new wxButton( this, ID_OKBUTTON, OKtext, wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer->Add( buttonOK, 0, wxALIGN_RIGHT|wxALL, 5 );


    this->SetSizer( fgSizer );
    this->Layout();

    this->CenterOnParent();
}

void YUVConfigDlg::OnOK(wxCommandEvent& event)
{
    wxString str;
    str = m_textCtrlW->GetValue();
    if(str.Len() == 0)
    {
        wxMessageBox(_T("Width is empty!"));
        return;
    }
    m_iWith = wxAtoi(str);
    str = m_textCtrlH->GetValue();
    if(str.Len() == 0)
    {
        wxMessageBox(_T("Height is empty!"));
        return;
    }
    m_iHeight = wxAtoi(str);
    int sel = m_choiceBit->GetSelection();
    m_b10bit = (sel != 0 ? true : false);
    Close();
    SetReturnCode(wxID_OK);
}

void YUVConfigDlg::OnCancel(wxCommandEvent& event)
{
    Close();
    SetReturnCode(wxID_CANCEL);
}

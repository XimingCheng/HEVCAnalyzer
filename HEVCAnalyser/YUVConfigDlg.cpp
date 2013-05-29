#include "YUVConfigDlg.h"

BEGIN_EVENT_TABLE(YUVConfigDlg, wxDialog)
    EVT_BUTTON(wxID_OK, YUVConfigDlg::OnOK)
END_EVENT_TABLE()

YUVConfigDlg::YUVConfigDlg(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, wxString(wxT("YUV Param Settings")))
{
    SetSize(250, 180);
    wxFlexGridSizer* fgSizer;
    fgSizer = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer->SetFlexibleDirection( wxBOTH );
    fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString s;
    wxString str;
    for(int i = 0; i < 10; i++)
    {
        str.Printf(wxT("%d"), i);
        s.Add(str);
    }
    tv.SetIncludes(s);

    wxStaticText* staticTextW = new wxStaticText( this, wxID_ANY, wxT("YUV Width"), wxDefaultPosition, wxDefaultSize, 0 );
    staticTextW->Wrap( -1 );
    fgSizer->Add( staticTextW, 0, wxALIGN_CENTER|wxALL, 5 );

    m_textCtrlW = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, tv );
    fgSizer->Add( m_textCtrlW, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText* staticTextH = new wxStaticText( this, wxID_ANY, wxT("YUV Height"), wxDefaultPosition, wxDefaultSize, 0 );
    staticTextH->Wrap( -1 );
    fgSizer->Add( staticTextH, 0, wxALIGN_CENTER|wxALL, 5 );

    m_textCtrlH = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, tv );
    fgSizer->Add( m_textCtrlH, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText* staticTextBit = new wxStaticText( this, wxID_ANY, wxT("YUV Bits"), wxDefaultPosition, wxDefaultSize, 0 );
    staticTextBit->Wrap( -1 );
    fgSizer->Add( staticTextBit, 0, wxALIGN_CENTER|wxALL, 5 );

    wxArrayString choiceBitStr;
    choiceBitStr.Add(wxT("8-Bit  YUV420"));
    choiceBitStr.Add(wxT("10-Bit YUV420"));
    m_choiceBit = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choiceBitStr, 0 );
    m_choiceBit->SetSelection( 0 );
    fgSizer->Add( m_choiceBit, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton* buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer->Add( buttonCancel, 0, wxALL, 5 );

    wxButton* buttonOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
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
        wxMessageBox(wxT("Width is empty!"));
        return;
    }
    m_iWith = wxAtoi(str);
    str = m_textCtrlH->GetValue();
    if(str.Len() == 0)
    {
        wxMessageBox(wxT("Height is empty!"));
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

#ifndef YUVCONFIGDLG_H_INCLUDED
#define YUVCONFIGDLG_H_INCLUDED

#include "HEVCAnalyzer.h"

class YUVConfigDlg : public wxDialog
{
public:
    enum
    {
        ID_OKBUTTON = wxID_HIGHEST+1,
        ID_CANCELBUTTON,
    };
    YUVConfigDlg(wxWindow *parent);

    int GetWidth()    const { return m_iWith; }
    int GetHeight()   const { return m_iHeight; }
    bool Is10bitYUV() const { return m_b10bit; }

    void SetWidth(const int w)    { m_iWith = w; }
    void SetHeight(const int h)   { m_iHeight = h; }
    void SetBitFlag(const bool b) { m_b10bit = b; }

    void SetWidth(const wxString& w) { m_textCtrlW->SetValue(w); }
    void SetHeight(const wxString& h) { m_textCtrlH->SetValue(h); }
private:
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

private:
    int         m_iWith;
    int         m_iHeight;
    bool        m_b10bit;
    wxTextCtrl* m_textCtrlW;
    wxTextCtrl* m_textCtrlH;
    wxChoice*   m_choiceBit;

    DECLARE_EVENT_TABLE()
};

#endif // YUVCONFIGDLG_H_INCLUDED

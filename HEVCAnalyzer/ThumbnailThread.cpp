#include "ThumbnailThread.h"

extern const wxEventType wxEVT_ADDANIMAGE_THREAD;
extern const wxEventType wxEVT_END_THREAD;

void* ThumbnailThread::Entry()
{
    m_pImageList->RemoveAll();
    m_cYUVIO.open((char *)m_sYUVPath.mb_str(wxConvUTF8).data(), false, m_iYUVBit, m_iYUVBit, m_iYUVBit, m_iYUVBit);
    m_pcPicYuvOrg = new TComPicYuv;
    m_pcPicYuvOrg->create( m_iSourceWidth, m_iSourceHeight, 64, 64, 4 );

    wxBitmap bmp(m_iSourceWidth, m_iSourceHeight, 24);
    int pad[] = {0, 0};
    int frame = 0;
    long framenumbers = 0;
    while(!m_cYUVIO.isEof() && !m_cYUVIO.isFail() && !TestDestroy())
    {
        // here read may be failed
        if(!m_cYUVIO.read(m_pcPicYuvOrg, pad))
            break;
        g_tranformYUV2RGB(m_iSourceWidth, m_iSourceHeight, m_pcPicYuvOrg, m_iYUVBit, bmp, bmp, bmp, bmp);
        //bmp.SaveFile(wxString::Format(_("test_%d.bmp"), frame), wxBITMAP_TYPE_BMP);
        wxImage bimg = bmp.ConvertToImage();
        double scaleRate = 165.0/m_iSourceWidth;
        wxImage simg = bimg.Scale((int)m_iSourceWidth*scaleRate, (int)m_iSourceHeight*scaleRate);
        wxBitmap newbmp(simg);
        m_pImageList->Add(newbmp);
        //m_pFrame->ProcessEvent(event);
        // this method can be used in Linux
        framenumbers++;
        if(framenumbers >= m_iFrameNumbers)
        {
            wxCommandEvent event(wxEVT_ADDANIMAGE_THREAD, wxID_ANY);
            event.SetExtraLong(framenumbers);
            event.SetInt(frame);
            wxPostEvent(m_pFrame, event);
            framenumbers = 0;
            frame++;
            continue;
        }
        frame++;
    }
    if(framenumbers > 0)
    {
        wxCommandEvent event(wxEVT_ADDANIMAGE_THREAD, wxID_ANY);
        event.SetExtraLong(framenumbers);
        event.SetInt(frame-1);
        wxPostEvent(m_pFrame, event);
    }
    m_cYUVIO.close();
    m_pcPicYuvOrg->destroy();
    delete m_pcPicYuvOrg;
    m_pcPicYuvOrg = NULL;
    wxCommandEvent event(wxEVT_END_THREAD, wxID_ANY);
    wxPostEvent(m_pFrame, event);
    return (wxThread::ExitCode)0;
}

ThumbnailThread::~ThumbnailThread()
{
    if(m_cYUVIO.isOpen())
        m_cYUVIO.close();
    if(m_pcPicYuvOrg)
    {
        m_pcPicYuvOrg->destroy();
        delete m_pcPicYuvOrg;
        m_pcPicYuvOrg = NULL;
    }
}

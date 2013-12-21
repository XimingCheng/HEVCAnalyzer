#include "ThumbnailThread.h"

extern const wxEventType wxEVT_ADDANIMAGE_THREAD;
extern const wxEventType wxEVT_END_THREAD;

double ThumbnailThread::m_fixWidth       = 165.0;
double ThumbnailThread::m_dOneAddingTime = 1500;

void* ThumbnailThread::Entry()
{
    m_pImageList->RemoveAll();
    m_cYUVIO.open((char *)m_sYUVPath.mb_str(wxCSConv(wxT("gb2312"))).data(), false, m_iYUVBit, m_iYUVBit, m_iYUVBit, m_iYUVBit);
    m_pcPicYuvOrg = new TComPicYuv;
    m_pcPicYuvOrg->create( m_iSourceWidth, m_iSourceHeight, 64, 64, 4 );
    int sizeYUV = wxFile((const wxChar*)m_sYUVPath).Length();
    int bytes = m_iYUVBit > 8 ? 2 : 1;
    int frames_num = sizeYUV/(m_iSourceWidth*m_iSourceHeight*1.5*bytes);
    if(frames_num < 6) m_bAdaptiveFrameNumbers = false;

    wxBitmap bmp(m_iSourceWidth, m_iSourceHeight, 24);
    int pad[] = {0, 0};
    int frame = 0;
    long framenumbers = 0;
    unsigned long long tstart = 0;
    unsigned long long tend = 0;
    if(m_bAdaptiveFrameNumbers)
    {
        tstart = g_getCurrentMS();
        m_iFrameNumbers = 5;
    }
    else
    {
        if(m_iFrameNumbers > frames_num)
            m_iFrameNumbers = frames_num;
    }
    while(frame < frames_num && !m_cYUVIO.isEof() && !m_cYUVIO.isFail() && !TestDestroy())
    {
        // here read may be failed
        if(!m_cYUVIO.read(m_pcPicYuvOrg, pad))
            break;
        g_tranformYUV2RGB(m_iSourceWidth, m_iSourceHeight, m_pcPicYuvOrg, m_iYUVBit, bmp, bmp, bmp, bmp);
        //bmp.SaveFile(wxString::Format(_("test_%d.bmp"), frame), wxBITMAP_TYPE_BMP);
        wxImage bimg = bmp.ConvertToImage();
        double scaleRate = m_fixWidth/m_iSourceWidth;
        wxImage simg = bimg.Scale((int)m_iSourceWidth*scaleRate, (int)m_iSourceHeight*scaleRate);
        wxBitmap newbmp(simg);
        if(!TestDestroy())
            m_pImageList->Add(newbmp);
        //m_pFrame->ProcessEvent(event);
        // this method can be used in Linux
        framenumbers++;
        if(framenumbers >= m_iFrameNumbers)
        {
            if(m_bAdaptiveFrameNumbers && tend == 0)
            {
                tend = g_getCurrentMS();
                m_iFrameNumbers = (int)5*m_dOneAddingTime/(tend - tstart);
                if(m_iFrameNumbers == 0)
                    m_iFrameNumbers = 1;
                if(m_iFrameNumbers > frames_num - frame - 1)
                    m_iFrameNumbers = frames_num - frame - 1;
                g_LogMessage(wxString::Format(_T("m_iFrameNumbers %d"), m_iFrameNumbers));
            }
            wxCommandEvent event(wxEVT_ADDANIMAGE_THREAD, wxID_ANY);
            event.SetExtraLong(framenumbers);
            event.SetInt(frame);
            if(!TestDestroy())
                wxPostEvent(m_pFrame, event);
            framenumbers = 0;
            frame++;
            continue;
        }
        frame++;
    }
    if((frame == frames_num || m_cYUVIO.isEof()) && framenumbers > 0)
    {
        wxCommandEvent event(wxEVT_ADDANIMAGE_THREAD, wxID_ANY);
        event.SetExtraLong(framenumbers);
        event.SetInt(frame-1);
        if(!TestDestroy())
            wxPostEvent(m_pFrame, event);
    }
    return (wxThread::ExitCode)0;
}

void ThumbnailThread::OnExit()
{
    //g_LogWarning(_T("ThumbnailThread::OnExit() called"));
    if(m_cYUVIO.isOpen())
        m_cYUVIO.close();
    if(m_pcPicYuvOrg)
    {
        m_pcPicYuvOrg->destroy();
        delete m_pcPicYuvOrg;
        m_pcPicYuvOrg = NULL;
    }
    if(!TestDestroy())
    {
        wxCommandEvent event(wxEVT_END_THREAD, wxID_ANY);
        wxPostEvent(m_pFrame, event);
    }
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

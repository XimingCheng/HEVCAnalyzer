#include "ThumbnailThread.h"
#include "MainUIInstance.h"
#include "../TAppDecoder/TAppDecTop.h"

bool g_md5_mismatch = false;

extern const wxEventType wxEVT_ADDANIMAGE_THREAD;
extern const wxEventType wxEVT_END_THREAD;

char opt0[] = "HEVCAnalyzer";
char opt1[] = "-b";
char opt3[] = "-o";
char opt5[] = "--OutputBitDepth=8";

double ThumbnailThread::m_fixWidth       = 165.0;
double ThumbnailThread::m_dOneAddingTime = 1500;

void* ThumbnailThread::Entry()
{
    m_pImageList->RemoveAll();
#if defined(__WXMSW__)
    m_cYUVIO.open(m_sYUVPath.mb_str(wxCSConv(wxFONTENCODING_SYSTEM)).data(), false, m_iYUVBit, m_iYUVBit, m_iYUVBit, m_iYUVBit);
#else
    m_cYUVIO.open(m_sYUVPath.mb_str(wxConvUTF8).data(), false, m_iYUVBit, m_iYUVBit, m_iYUVBit, m_iYUVBit);
#endif
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
                LogMsgUIInstance::GetInstance()->LogMessage(wxString::Format(_T("m_iFrameNumbers %d"), m_iFrameNumbers));
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

void* DecodingThread::Entry()
{
    GenerateCommandLine();
    g_md5_mismatch = false;
    TAppDecTop cTAppDecTop;
    cTAppDecTop.create();

    if(!cTAppDecTop.parseCfg( m_argcofDecoder, m_argvofDecoder ))
    {
        cTAppDecTop.destroy();
        ReleaseBuffer();
        return (wxThread::ExitCode)1;
    }
    // main HEVC decoder
    cTAppDecTop.decode();
    if(g_md5_mismatch)
        wxMessageBox(_T("A decoding mismatch occured: signalled md5sum does not match"),
            _T("Decoding Error"), wxICON_ERROR);
    cTAppDecTop.destroy();
    ReleaseBuffer();
    return g_md5_mismatch ? (wxThread::ExitCode)1 : (wxThread::ExitCode)0;
}

void DecodingThread::ReleaseBuffer()
{
    if(m_argvofDecoder)
    {
        delete [] m_argvofDecoder;
        m_argvofDecoder = NULL;
    }
    if(m_pStreamPathAnsi)
    {
        delete [] m_pStreamPathAnsi;
        m_pStreamPathAnsi = NULL;
    }
    if(m_pOutYUVPathAnsi)
    {
        delete [] m_pOutYUVPathAnsi;
        m_pOutYUVPathAnsi = NULL;
    }
}

void DecodingThread::OnExit()
{
    ReleaseBuffer();
}

DecodingThread::~DecodingThread()
{
    ReleaseBuffer();
}

void DecodingThread::GenerateCommandLine()
{
    if(!m_b10bit)
        m_argcofDecoder = 6;
    else
        m_argcofDecoder = 5;

    m_argvofDecoder = new char*[m_argcofDecoder];
#if defined(__WXMSW__)
    wxCharBuffer buffStreamPath = m_sStreamPath.mb_str(wxCSConv(wxFONTENCODING_SYSTEM));
    wxCharBuffer buffOutYUVPath = m_sOutYUVPath.mb_str(wxCSConv(wxFONTENCODING_SYSTEM));
#else
    wxCharBuffer buffStreamPath = m_sStreamPath.mb_str(wxConvUTF8);
    wxCharBuffer buffOutYUVPath = m_sOutYUVPath.mb_str(wxConvUTF8);
#endif
    const char* pStreamData = buffStreamPath.data();
    const char* pOutYUVData = buffOutYUVPath.data();
    m_pStreamPathAnsi = new char[strlen(pStreamData) + 1];
    m_pOutYUVPathAnsi = new char[strlen(pOutYUVData) + 1];
    strcpy(m_pStreamPathAnsi, pStreamData);
    strcpy(m_pOutYUVPathAnsi, pOutYUVData);
    m_argvofDecoder[0] = opt0;
    m_argvofDecoder[1] = opt1;
    m_argvofDecoder[2] = m_pStreamPathAnsi;
    m_argvofDecoder[3] = opt3;
    m_argvofDecoder[4] = m_pOutYUVPathAnsi;
    if(!m_b10bit)
        m_argvofDecoder[5] = opt5;
}

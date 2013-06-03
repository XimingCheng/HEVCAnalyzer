#include "ThumbnailThread.h"

extern const wxEventType wxEVT_ADDANIMAGE_THREAD;
extern const wxEventType wxEVT_END_THREAD;

void* ThumbnailThread::Entry()
{
    m_pImageList->RemoveAll();
    TVideoIOYuv cYUVIO;
    cYUVIO.open((char *)m_sYUVPath.mb_str(wxConvUTF8).data(), false, m_iYUVBit, m_iYUVBit, m_iYUVBit, m_iYUVBit);
    m_pcPicYuvOrg = new TComPicYuv;
    m_pcPicYuvOrg->create( m_iSourceWidth, m_iSourceHeight, 64, 64, 4 );

    wxBitmap bmp(m_iSourceWidth, m_iSourceHeight, 24);
    int frame = 0;
    while(!cYUVIO.isEof() && !TestDestroy())
    {
        int pad[] = {0, 0};
        cYUVIO.read(m_pcPicYuvOrg, pad);
        wxNativePixelData img(bmp);
        wxNativePixelData::Iterator p(img);
        for(int j = 0; j < m_iSourceHeight; j++)
        {
            wxNativePixelData::Iterator rowStart = p;
            Pel* pY = m_pcPicYuvOrg->getLumaAddr() + j*m_pcPicYuvOrg->getStride();
            Pel* pU = m_pcPicYuvOrg->getCbAddr()   + (j/2)*m_pcPicYuvOrg->getCStride();
            Pel* pV = m_pcPicYuvOrg->getCrAddr()   + (j/2)*m_pcPicYuvOrg->getCStride();
            for(int i = 0; i < m_iSourceWidth; i++)
            {
                unsigned char y, u, v;
                if(m_iYUVBit == 10)
                {
                    y = (unsigned char)(pY[i] >> 2);
                    u = (unsigned char)(pU[i/2] >> 2);
                    v = (unsigned char)(pV[i/2] >> 2);
                }
                else
                {
                    y = (unsigned char)pY[i];
                    u = (unsigned char)pU[i/2];
                    v = (unsigned char)pV[i/2];
                }
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
        double scaleRate = 165.0/m_iSourceWidth;
        wxImage simg = bimg.Scale((int)m_iSourceWidth*scaleRate, (int)m_iSourceHeight*scaleRate);
        wxBitmap newbmp(simg);
        m_pImageList->Add(newbmp);
        wxCommandEvent event(wxEVT_ADDANIMAGE_THREAD, wxID_ANY);
        event.SetInt(frame);
        //m_pFrame->ProcessEvent(event);
        // this method can be used in Linux
        wxPostEvent(m_pFrame, event);
        frame++;
    }

    m_pcPicYuvOrg->destroy();
    delete m_pcPicYuvOrg;
    m_pcPicYuvOrg = NULL;
    wxCommandEvent event(wxEVT_END_THREAD, wxID_ANY);
    wxPostEvent(m_pFrame, event);
    return (wxThread::ExitCode)0;
}

ThumbnailThread::~ThumbnailThread()
{
    if(m_pcPicYuvOrg)
    {
        m_pcPicYuvOrg->destroy();
        delete m_pcPicYuvOrg;
        m_pcPicYuvOrg = NULL;
    }
}

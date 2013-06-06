#include "HEVCAnalyser.h"

wxTextCtrl *g_pLogWin = NULL;

void g_tranformYUV2RGB(int w, int h, TComPicYuv*  pcPicYuvOrg, int iYUVBit, wxBitmap& bmp)
{
    wxNativePixelData img(bmp);
    wxNativePixelData::Iterator p(img);
    for(int j = 0; j < h; j++)
    {
        wxNativePixelData::Iterator rowStart = p;
        Pel* pY = pcPicYuvOrg->getLumaAddr() + j*pcPicYuvOrg->getStride();
        Pel* pU = pcPicYuvOrg->getCbAddr()   + (j/2)*pcPicYuvOrg->getCStride();
        Pel* pV = pcPicYuvOrg->getCrAddr()   + (j/2)*pcPicYuvOrg->getCStride();
        for(int i = 0; i < w; i++)
        {
            unsigned char y, u, v;
            if(iYUVBit == 10)
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
}

void g_LogMessage(wxString message)
{
    if(g_pLogWin==NULL)
        wxLogMessage(message);
    else
    {
        message.Trim();
        long startpos = g_pLogWin->GetLastPosition();
        g_pLogWin->WriteText(wxDateTime::Now().FormatTime()+_T(" [Message] ")+message+_T("\n"));
        long endpos = g_pLogWin->GetLastPosition();
        g_pLogWin->SetStyle(startpos, endpos, *wxGREEN);
    }
}

void g_LogError(wxString error)
{
    if(g_pLogWin==NULL)
        wxLogError(error);
    else
    {
        long startpos = g_pLogWin->GetLastPosition();
        error.Trim();
        g_pLogWin->WriteText(wxDateTime::Now().FormatTime()+_T(" [ Error ] ")+error+_T("\n"));
        long endpos = g_pLogWin->GetLastPosition();
        g_pLogWin->SetStyle(startpos, endpos, *wxRED);
    }
}

void g_LogWarning(wxString warning)
{
    if(g_pLogWin==NULL)
        wxLogWarning(warning);
    else
    {
        long startpos = g_pLogWin->GetLastPosition();
        warning.Trim();
        g_pLogWin->WriteText(wxDateTime::Now().FormatTime()+_T(" [Warning] ")+warning+_T("\n"));
        long endpos = g_pLogWin->GetLastPosition();
        g_pLogWin->SetStyle(startpos, endpos, wxColour(174, 174, 0));
    }
}

void g_SetActiveTarget(wxTextCtrl *pTC)
{
    g_pLogWin = pTC;
}

void g_ClearLog()
{
    if (g_pLogWin != NULL)
        g_pLogWin->Clear();
}

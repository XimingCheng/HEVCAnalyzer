#include "HEVCAnalyzer.h"

wxTextCtrl *g_pLogWin = NULL;

void g_tranformYUV2RGB(const int w, const int h, TComPicYuv* pcPicYuvOrg, const int iYUVBit,
                       wxBitmap& bmp, wxBitmap& bmp_Y, wxBitmap& bmp_U, wxBitmap& bmp_V, bool bUseYUV)
{
    wxNativePixelData img(bmp);
    wxNativePixelData::Iterator p(img);
    wxNativePixelData imgY(bmp_Y);
    wxNativePixelData::Iterator pIY(imgY);
    wxNativePixelData imgU(bmp_U);
    wxNativePixelData::Iterator pIU(imgU);
    wxNativePixelData imgV(bmp_V);
    wxNativePixelData::Iterator pIV(imgV);
    for(int j = 0; j < h; j++)
    {
        wxNativePixelData::Iterator rowStart  = p;
        wxNativePixelData::Iterator rowStartY = pIY;
        wxNativePixelData::Iterator rowStartU = pIU;
        wxNativePixelData::Iterator rowStartV = pIV;

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
            if(bUseYUV)
            {
                pIY.Red() = y;
                pIY.Green() = y;
                pIY.Blue() = y;
                pIU.Red() = u;
                pIU.Green() = u;
                pIU.Blue() = u;
                pIV.Red() = v;
                pIV.Green() = v;
                pIV.Blue() = v;
                pIY++;
                pIU++;
                pIV++;
            }
            p++;
        }
        p = rowStart;
        p.OffsetY(img, 1);
        if(bUseYUV)
        {
            pIY = rowStartY;
            pIY.OffsetY(imgY, 1);
            pIU = rowStartU;
            pIU.OffsetY(imgU, 1);
            pIV = rowStartV;
            pIV.OffsetY(imgV, 1);
        }
    }
}

bool g_parseResolutionFromFilename(const wxString &filename, wxString  &width, wxString &height)
{
    wxString  Regex = _T("_[0-9]+x[0-9]+_");
    wxString  ResolutionString;
    wxRegEx reResolution(Regex);
    wxRegEx reNum(_T("[0-9]+"));

    if ( reResolution.Matches(filename) )
    {
        ResolutionString = reResolution.GetMatch(filename);
        if ( reNum.Matches(ResolutionString) )
        {
            width = reNum.GetMatch(ResolutionString);
        }
        ResolutionString = ResolutionString.AfterFirst(_T('x'));
        if ( reNum.Matches(ResolutionString) )
        {
            height = reNum.GetMatch(ResolutionString);
        }
        return true;
    }
    return false;
}

unsigned long long g_getCurrentMS()
{
    wxDateTime t = wxDateTime::UNow();
    return t.GetTicks()*1000 + t.GetMillisecond();
}

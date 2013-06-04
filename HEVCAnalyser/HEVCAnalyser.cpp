#include "HEVCAnalyser.h"

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

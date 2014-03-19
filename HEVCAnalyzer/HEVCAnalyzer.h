#ifndef HEVCANALYZER_H_INCLUDED
#define HEVCANALYZER_H_INCLUDED

#include <cmath>
#include <cassert>

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibook.h>
#include <wx/listctrl.h>
#include <wx/file.h>
#include <wx/rawbmp.h>
#include <wx/thread.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/fs_mem.h>
#include <wx/htmllbox.h>
#include <wx/imaglist.h>
#include <wx/arrstr.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <wx/regex.h>
#include <wx/stdpaths.h>
#include <wx/wxsqlite3.h>

#include "../TLibVideoIO/TVideoIOYuv.h"

#define MINDIFF    1e-15

void               g_tranformYUV2RGB(const int w, const int h, TComPicYuv* pcPicYuvOrg, const int iYUVBit,
                        wxBitmap& bmp, wxBitmap& bmp_Y, wxBitmap& bmp_U, wxBitmap& bmp_V, bool bUseYUV = false);
bool               g_parseResolutionFromFilename(const wxString &filename, wxString  &width, wxString &height);
unsigned long long g_getCurrentMS();

#endif // HEVCANALYZER_H_INCLUDED

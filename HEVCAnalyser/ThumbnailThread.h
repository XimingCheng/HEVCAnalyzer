#ifndef THUMBNAILTHREAD_H_INCLUDED
#define THUMBNAILTHREAD_H_INCLUDED

#include "HEVCAnalyser.h"
#include "../TLibVideoIO/TVideoIOYuv.h"
#include "HEVCodecApp.h"

class MainFrame;

class ThumbnailThread : public wxThread
{
public:
    ThumbnailThread(MainFrame* pFrame, wxImageList* pImageList, int w, int h, int bit, const wxString& path)
    : m_sYUVPath(path), m_iSourceWidth(w), m_iSourceHeight(h), m_iYUVBit(bit), m_pImageList(pImageList),
      m_pFrame(pFrame), m_pcPicYuvOrg(NULL)
    { }
    ~ThumbnailThread();

protected:
    virtual void* Entry();

    wxString     m_sYUVPath;
    int          m_iSourceWidth;
    int          m_iSourceHeight;
    int          m_iYUVBit;
    wxImageList* m_pImageList;
    MainFrame*   m_pFrame;
    TComPicYuv*  m_pcPicYuvOrg;
};

#endif // THUMBNAILTHREAD_H_INCLUDED

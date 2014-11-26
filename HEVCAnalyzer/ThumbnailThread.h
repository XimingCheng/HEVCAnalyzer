#ifndef THUMBNAILTHREAD_H_INCLUDED
#define THUMBNAILTHREAD_H_INCLUDED

#include "HEVCAnalyzer.h"
#include "HEVCodecApp.h"

class ThumbnailThread : public wxThread
{
public:
    ThumbnailThread(wxFrame* pFrame, wxImageList* pImageList, int w, int h, int bit, const wxString& path, int framenumbers=10)
    : m_sYUVPath(path), m_iSourceWidth(w), m_iSourceHeight(h), m_iYUVBit(bit), m_pImageList(pImageList), m_pFrame(pFrame),
      m_iFrameNumbers(framenumbers), m_pcPicYuvOrg(NULL), m_bAdaptiveFrameNumbers(true)
    { }
    ~ThumbnailThread();
    void SetAdaptiveFrameNumbers(bool b) { m_bAdaptiveFrameNumbers = b; }
    bool GetAdaptiveFrameNumbers() const { return m_bAdaptiveFrameNumbers; }

protected:
    virtual void* Entry() override;
    void OnExit() override;

    wxString     m_sYUVPath;
    int          m_iSourceWidth;
    int          m_iSourceHeight;
    int          m_iYUVBit;
    wxImageList* m_pImageList;
    wxFrame*     m_pFrame;

    //when the thread have processed m_iFrameNumbers frames
    //and then send message to the main thread.
    int          m_iFrameNumbers;
    TComPicYuv*  m_pcPicYuvOrg;
    TVideoIOYuv  m_cYUVIO;
    bool         m_bAdaptiveFrameNumbers;
    static double m_fixWidth;
    static double m_dOneAddingTime;
};

class DecodingThread : public wxThread
{
public:
    DecodingThread(wxFrame* pFrame, const wxString& streamPath, const wxString& outYUVPath, bool b10bit = false)
    : m_sStreamPath(streamPath), m_sOutYUVPath(outYUVPath), m_b10bit(b10bit),
      m_pFrame(pFrame), m_argcofDecoder(0), m_argvofDecoder(NULL), m_pStreamPathAnsi(NULL), m_pOutYUVPathAnsi(NULL)
    { }
    ~DecodingThread();
    void SetStreamPath(const wxString& path) { m_sStreamPath = path; }
    void SetOutputYUVpath(const wxString& path) { m_sOutYUVPath = path; }
    void SetYUVBitsFlag(bool b10bit) { m_b10bit = b10bit; }

protected:
    virtual void* Entry() override;
    void          ReleaseBuffer();
    void          GenerateCommandLine();
    void          OnExit() override;

    wxString      m_sStreamPath;
    wxString      m_sOutYUVPath;
    bool          m_b10bit;
    wxImageList*  m_pImageList;
    wxFrame*      m_pFrame;
    int           m_argcofDecoder;
    char**        m_argvofDecoder;
    char*         m_pStreamPathAnsi;
    char*         m_pOutYUVPathAnsi;
};

#endif // THUMBNAILTHREAD_H_INCLUDED

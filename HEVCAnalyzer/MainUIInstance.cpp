#include "MainUIInstance.h"

MainUIInstance*   MainUIInstance::m_pInstance   = NULL;
LogMsgUIInstance* LogMsgUIInstance::m_pInstance = NULL;
wxMutex MainUIInstance::m_Mutex;
wxMutex LogMsgUIInstance::m_Mutex;

extern const wxEventType wxEVT_LOGMSG;
extern const wxEventType wxEVT_DECODING_MAINFRAME_NOTIFY;

MainUIInstance* MainUIInstance::GetInstance()
{
    wxMutexLocker mutexLocker(m_Mutex);
    if(!m_pInstance)
        m_pInstance = new MainUIInstance();
    return m_pInstance;
}

void MainUIInstance::Destroy()
{
    wxMutexLocker mutexLocker(m_Mutex);
    if(m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

void MainUIInstance::SetActiveTargetMainFrame(wxFrame* pFrame)
{
    m_pMainFrame = pFrame;
}

LogMsgUIInstance* LogMsgUIInstance::GetInstance()
{
    wxMutexLocker mutexLocker(m_Mutex);
    if(!m_pInstance)
        m_pInstance = new LogMsgUIInstance();
    return m_pInstance;
}

void LogMsgUIInstance::Destory()
{
    wxMutexLocker mutexLocker(m_Mutex);
    if(m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

void LogMsgUIInstance::SetActiveTarget(wxFrame *pFrame)
{
    m_pFrame = pFrame;
}

void LogMsgUIInstance::LogMessage(const wxString& msg, MSG_TYPE type)
{
    if(!m_pFrame)
        wxLogMessage(msg);
    else
    {
        wxCommandEvent event(wxEVT_LOGMSG, wxID_ANY);
        event.SetInt((int)type);
        event.SetString(msg);
        wxPostEvent(m_pFrame, event);
    }
}

void  LogMsgUIInstance::ClearLog()
{
    if(m_pFrame)
    {
        wxCommandEvent event(wxEVT_LOGMSG, wxID_ANY);
        event.SetInt((int)MSG_TYPE_CLEAR);
        wxPostEvent(m_pFrame, event);
    }
}

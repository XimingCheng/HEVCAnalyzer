#ifndef MAINUIINSTANCE_H_INCLUDED
#define MAINUIINSTANCE_H_INCLUDED

#include "MainFrame.h"

class NoneCopyable
{
protected:
    // = delete functions
    NoneCopyable() {}
    NoneCopyable(const NoneCopyable& ins);
    NoneCopyable& operator = (const NoneCopyable& ins);
};

class MainUIInstance : public NoneCopyable
{
public:
    static MainUIInstance*   GetInstance();
    static void              Destroy();

private:
    // private data member
    static MainUIInstance*   m_pInstance;
    static wxMutex           m_Mutex;
    MainFrame*               m_pMainFrame;
    CenterPageManager*       m_pCenterPageManger;
    wxSQLite3Database*       m_pDataBase;
};

class LogMsgUIInstance : public NoneCopyable
{
public:
    enum MSG_TYPE
    {
        MSG_TYPE_NORMAL = 0,
        MSG_TYPE_WARNING,
        MSG_TYPE_ERROR,
        MSG_TYPE_CLEAR,
        MSG_TYPE_MAX,
    };
    static LogMsgUIInstance* GetInstance();
    static void              Destory();
    void                     SetActiveTarget(wxFrame *pFrame);
    void                     LogMessage(const wxString& msg, MSG_TYPE type = MSG_TYPE_NORMAL);
    void                     ClearLog();

private:
    LogMsgUIInstance() : NoneCopyable(), m_pFrame(NULL) {}
    static LogMsgUIInstance* m_pInstance;
    static wxMutex           m_Mutex;
    wxFrame*                 m_pFrame;
};

#endif // MAINUIINSTANCE_H_INCLUDED

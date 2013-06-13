#ifndef PIXELVIEWCTRL_H_INCLUDED
#define PIXELVIEWCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"

class PixelViewCtrl : public wxScrolledWindow
{
    public:
        DECLARE_DYNAMIC_CLASS(PixelViewCtrl);
        PixelViewCtrl(){}
        PixelViewCtrl(wxWindow *parent, wxWindowID id)
            :wxScrolledWindow(parent, id), m_FocusPos(10, 10),
            m_iCUHeight(64), m_iCUWidth(64), m_iHeightPerPixel(15),
            m_iWidthPerPixel(30)
        {
            SetBackgroundStyle(wxBG_STYLE_CUSTOM);
            //SetBackgroundColour(*wxWHITE);
            SetScrollRate(5, 5);
            SetVirtualSize(20+m_iCUWidth*m_iWidthPerPixel,
                           20+m_iCUHeight*m_iHeightPerPixel);
        }

        void OnDraw(wxDC&);
        void OnEraseBkg(wxEraseEvent& event);
        void DrawGrid(wxDC& dc);
        void DrawFocusLine(wxDC& dc);

        void SetFocusPos(const wxPoint& pos);
        void SetCUHeight(const int height){m_iCUHeight = height;}
        void SetCUWidth(const int width){m_iCUWidth = width;}
        void SetHeight(const int height){m_iHeightPerPixel = height;}
        void SetWidth(const int width){m_iWidthPerPixel = width;}

        int GetHeight()const{return m_iHeightPerPixel;}
        int GetWidth()const{return m_iWidthPerPixel;}
    private:
        wxPoint         m_FocusPos;
        int             m_iCUHeight;
        int             m_iCUWidth;
        int             m_iHeightPerPixel;
        int             m_iWidthPerPixel;
        bool            m_bHexFormat;

        DECLARE_EVENT_TABLE();
};

#endif

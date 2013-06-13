#ifndef PIXELVIEWCTRL_H_INCLUDED
#define PIXELVIEWCTRL_H_INCLUDED

#include "HEVCAnalyzer.h"

class PixelViewCtrl : public wxScrolledWindow
{
    public:
        DECLARE_DYNAMIC_CLASS(PixelViewCtrl);
        PixelViewCtrl(){}
        PixelViewCtrl(wxWindow *parent, wxWindowID id)
            :wxScrolledWindow(parent, id)
        {
            SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        }

        void OnDraw(wxDC&);
        void OnEraseBkg(wxEraseEvent& event);
        void DrawGrid(wxDC&);
        void DrawFocusLine(wxDC&);

        void SetFocusPos(const wxPoint& pos);
        void SetCUHeight(const int height){m_iCUHeight = heigh;}
        void SetCUWidth(const int width){m_iCUWidth = width;}
        void SetHeight(const int height){m_iHeightPerPixel = height;}
        void SetWidth(const int width){m_iWidthPerPixel = width;}

        int GetHeight(){return m_iHeightPerPixel;}
        int GetWidth(){return m_iWidthPerPixel;}
    private:
        wxPoint         m_FocusPos;
        int             m_iCUHeight;
        int             m_iCUWidth;
        int             m_iHeightPerPixel;
        int             m_iWidthPerPixel;
        wxPen           m_FocusPen;
        DECLARE_EVENT_TABLE();
};

#endif

#include <windows.h>

#define NoDepends

#define GDIPP_FILLRECT 0x1
#define GDIPP_REDRAW 0x2
#define GDIPP_INVALIDATE 0x3

#define SafeReleaseDC(Wnd, DC) __pragma(warning(disable:6387)) if((DC != NULL && DC != INVALID_HANDLE_VALUE) && Wnd != NULL) { ReleaseDC(Wnd, DC); } __pragma(warning(default:6387))
#define SafeDeleteDC(DC) __pragma(warning(disable:6387)) if(DC != NULL && DC != INVALID_HANDLE_VALUE) { DeleteDC(DC); } __pragma(warning(default:6387))
#define SafeDeleteObject(Obj) __pragma(warning(disable:6387)) if(Obj != NULL && Obj != INVALID_HANDLE_VALUE) { DeleteObject(Obj); } __pragma(warning(default:6387))



class BrushPP
{
public:

    BrushPP()
    {
        if(NeedsDestroyed)
        {
            if(Brush != NULL)
            {
                DeleteObject(Brush);
            }
        }

        NeedsDestroyed = false;
    }

    BrushPP(const HBRUSH HBrush)
    {
        if(NeedsDestroyed == true)
        {
            if(Brush != NULL)
            {
              DeleteObject(Brush);
            }
        }
        if(HBrush != NULL)
        {
            Brush = HBrush;
            NeedsDestroyed = true;
        }
        else
        {
            NeedsDestroyed = false;
        }
    }

    BrushPP(const COLORREF Clr)
    {
        if(NeedsDestroyed == true)
        {
            if(Brush != NULL)
            {
                DeleteObject(Brush);
            }
        }
        Brush = CreateSolidBrush(Clr);
        NeedsDestroyed = true;
    }

    BrushPP(const int Hatch, const COLORREF Clr)
    {
        if(NeedsDestroyed == true)
        {
            if(Brush != NULL)
            {
                DeleteObject(Brush);
            }
        }
        Brush = CreateHatchBrush(Hatch, Clr);
        NeedsDestroyed = true;
    }

    BrushPP(const HBITMAP Bmp)
    {
        if(NeedsDestroyed == true)
        {
            if(Brush != NULL)
            {
                DeleteObject(Brush);
            }
        }
        Brush = CreatePatternBrush(Bmp);
        NeedsDestroyed = true;
    }

    // Disable copy assignment && Copy construct
    BrushPP(const BrushPP&) = delete;
    BrushPP& operator=(const BrushPP&) = delete;

    // Move constructor
    BrushPP(BrushPP&& Rhs) noexcept
    {
       if(Rhs.Brush != NULL)
        {
            Brush = Rhs.Brush;
            Rhs.Brush = nullptr;
        }
    }

    // Destructor
    ~BrushPP()
    {
        if(NeedsDestroyed)
        {
            if(Brush != NULL)
            {
                DeleteObject(Brush);
            }
            NeedsDestroyed = false;
        }
    }

    operator HBRUSH() const
    {
        return Brush;
    }

    explicit operator bool() const noexcept
    {
        if(Brush != NULL)
            return true;
        return false;
    }

    // Move assignment
    BrushPP& operator=(BrushPP&& Rhs) noexcept
    {
        if(this != &Rhs)
        {
            if(NeedsDestroyed == true)
            {
                if(Brush != NULL)
                {
                    DeleteObject(Brush);
                }
            }

            Brush = Rhs.Brush;
            Rhs.Brush = nullptr;
            NeedsDestroyed = true;
        }
        return *this;
    }

private:
    HBRUSH Brush{};
    bool NeedsDestroyed = false;
};

class PenPP
{
public:

    PenPP()
    {
        if(NeedsDestroyed)
        {
            if(Pen != NULL)
            {
                DeleteObject(Pen);
            }
        }

        NeedsDestroyed = false;
    }

    PenPP(const HPEN HPen)
    {
        if(NeedsDestroyed == true)
        {
            if(Pen != NULL)
            {
                DeleteObject(Pen);
            }
        }
        if(HPen != NULL)
        {
            Pen = HPen;
            NeedsDestroyed = true;
        }
        else
        {
            NeedsDestroyed = false;
        }
    }

    PenPP(const int Style, const int Width, COLORREF Clr)
    {
        if(NeedsDestroyed == true)
        {
            if(Pen != NULL)
            {
                DeleteObject(Pen);
            }
        }
        Pen = CreatePen(Style, Width, Clr);
        NeedsDestroyed = true;
    }

    // Disable copy assignment && Copy construct
    PenPP(const PenPP&) = delete;
    PenPP& operator=(const PenPP&) = delete;

    // Move constructor
    PenPP(PenPP&& Rhs) noexcept
    {
        if(Rhs.Pen != NULL)
        {
            Pen = Rhs.Pen;
            Rhs.Pen = nullptr;
        }
    }

    // Destructor
    ~PenPP()
    {
        if(NeedsDestroyed)
        {
            if(Pen != NULL)
            {
                DeleteObject(Pen);
            }
            NeedsDestroyed = false;
        }
    }

    operator HPEN() const
    {
        return Pen;
    }

    explicit operator bool() const
    {
        if(Pen != NULL && Pen != INVALID_HANDLE_VALUE)
            return true;
        return false;
    }

    // Move assignment
    PenPP& operator = (PenPP&& Rhs) noexcept
    {
        if(this != &Rhs)
        {
            if(NeedsDestroyed == true)
            {
                if(Pen != NULL)
                {
                    DeleteObject(Pen);
                }
            }

            Pen = Rhs.Pen;
            Rhs.Pen = nullptr;
            NeedsDestroyed = true;
        }
        return *this;
    }
private:
    HPEN Pen{};
    bool NeedsDestroyed = false;
};

class GdiPP
{
public:
    static void LogError(std::string ErrorMsg)
    {
        std::cout << ErrorMsg << std::endl;
    }

    GdiPP(const HWND& Wnd, const bool IsDoubleBuffered = false) // ctor
    {
        DoubleBuffered = IsDoubleBuffered;
        this->Wnd = Wnd;
        
        ScreenDC = GetDC(Wnd);

        if(!ScreenDC)
        {
            ErrorHandler("[GdiPP]  Failed to retrieve dc from HWND");
            SafeReleaseDC(Wnd, ScreenDC);
            return;
        }

        GetClientRect(Wnd, &ClientRect);
        
        if(IsDoubleBuffered == true)
        {
            MemDC = CreateCompatibleDC(ScreenDC);
            MemBM = CreateCompatibleBitmap(ScreenDC, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top);
            OldBM = (HBITMAP)SelectObject(MemDC, MemBM);
            if(!MemDC || !MemBM || !OldBM)
            {
                ErrorHandler("[GdiPP]  Failed to initialize DoubleBuffering");
                SafeReleaseDC(Wnd, ScreenDC);
                SafeDeleteDC(MemDC);
                SafeDeleteObject(MemBM);
                SafeDeleteObject(OldBM);
            }
        }
    }

    // disable copy
    //GdiPP(const GdiPP&) = delete;
    //GdiPP& operator=(const GdiPP&) = delete;

    ~GdiPP() // dtor 
    {
        SelectObject(MemDC, OldBM);
        SelectObject(MemDC, OldPen);
        SelectObject(MemDC, OldBrush);
        SafeDeleteObject(MemBM);
        SafeDeleteObject(OldBM);
        SafeDeleteObject(OldPen);
        SafeDeleteObject(OldBrush);
        SafeDeleteDC(MemDC);
        SafeReleaseDC(Wnd, ScreenDC);
    }

    bool ChangePen(const HPEN NewPen) 
    {
        if(!DoubleBuffered)
        {
            if(ScreenDC)
            {
                if(OldPen)
                {
                    SelectObject(ScreenDC, NewPen);
                }

                else
                {
                    OldPen = (HPEN)SelectObject(ScreenDC, NewPen);
                }

                if(!OldPen)
                {
                    ErrorHandler("[GdiPP]  Failed to create pen");
                    return false;
                }
            }
        }
        else
        {
            if(MemDC)
            {
                if(OldPen)
                {
                    SelectObject(MemDC, NewPen);
                }

                else
                {
                    OldPen = (HPEN)SelectObject(MemDC, NewPen);
                }

                if(!OldPen)
                {
                    ErrorHandler("[GdiPP]  Failed to create pen");
                    return false;
                }
            }
        }

        return true;
    }

    bool ChangeBrush(const HBRUSH NewBrush)
    {
        if(!DoubleBuffered)
        {
            if(ScreenDC)
            {
                if(OldBrush)
                {
                    SelectObject(ScreenDC, NewBrush);
                }
                else
                {
                    OldBrush = (HBRUSH)SelectObject(ScreenDC, NewBrush);
                }

                if(!OldBrush)
                {
                    ErrorHandler("[GdiPP]  Error vreating solid brush");
                    return false;
                }
            }
        }
        else
        {
            if(MemDC)
            {
                if(OldBrush)
                {
                    SelectObject(MemDC, NewBrush);
                }
                else
                {
                    OldBrush = (HBRUSH)SelectObject(MemDC, NewBrush);
                }

                if(!OldBrush)
                {
                    ErrorHandler("[GdiPP]  Error creating solid brush");
                    return false;
                }
            }
        }
        return true;
    }

    bool DrawRectangle(const int X, const int Y, const int Width, const int Height)
    {
        if(!DoubleBuffered)
        {
            if(!ScreenDC)
                return false;

            return Rectangle(ScreenDC, X, Y, X+Width, Y+Height);
        }
        else
        {
            if(!MemDC)
                return false;

            return Rectangle(MemDC, X, Y, X+Width, Y+Height);
        }
    }

    bool DrawStringA(const int X, const int Y, const std::string Text, const COLORREF TextColor, const int BkMode)
    {
        if(!DoubleBuffered)
        {
            if(!ScreenDC)
                return false;

            if(!SetBkMode(ScreenDC, BkMode))
            {
                ErrorHandler("[GdiPP]  Failed to set text background mode");
                return false;
            }
            if(SetTextColor(ScreenDC, TextColor) == CLR_INVALID)
            {
                ErrorHandler("[GdiPP]  Failed to set text color");
                return false;
            }
            return TextOutA(ScreenDC, X, Y, Text.c_str(), (int)Text.length());
        }
        else
        {
            if(!MemDC)
                return false;
            
            if(!SetBkMode(MemDC, BkMode))
            {
                ErrorHandler("[GdiPP]  Failed to set text background mode");
                return false;
            }
            if(SetTextColor(MemDC, TextColor) == CLR_INVALID)
            {
                ErrorHandler("[GdiPP]  Failed to set text color");
                return false;
            }
            return TextOutA(MemDC, X, Y, Text.c_str(), (int)Text.length());
        }
    }

    bool DrawStringW(const int X, const int Y, const std::wstring Text, const COLORREF TextColor, const int BkMode)
    {
        if (!DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            if (!SetBkMode(ScreenDC, BkMode))
            {
                ErrorHandler("[GdiPP]  Failed to set text background mode");
                return false;
            }
            if (SetTextColor(ScreenDC, TextColor) == CLR_INVALID)
            {
                ErrorHandler("[GdiPP]  Failed to set text color");
                return false;
            }
            return TextOutW(ScreenDC, X, Y, Text.c_str(), (int)Text.length());
        }
        else
        {
            if (!MemDC)
                return false;

            if (!SetBkMode(MemDC, BkMode))
            {
                ErrorHandler("[GdiPP]  Failed to set text background mode");
                return false;
            }
            if (SetTextColor(MemDC, TextColor) == CLR_INVALID)
            {
                ErrorHandler("[GdiPP]  Failed to set text color");
                return false;
            }
            return TextOutW(MemDC, X, Y, Text.c_str(), (int)Text.length());
        }
    }

    bool DrawLine(const int StartX, const int StartY, const int EndX, const int EndY)
    {
        bool Status = false;
        if(!DoubleBuffered)
        {
            if(!ScreenDC)
                return false;
            
            POINT OldPos;
            Status = MoveToEx(ScreenDC, StartX, StartY, &OldPos);
            Status = LineTo(ScreenDC, EndX, EndY);
            Status = MoveToEx(ScreenDC, OldPos.x, OldPos.y, nullptr);
            if(!Status)
            {
                ErrorHandler("[GdiPP]  Failed to DrawLine");
                return false;
            }
            return true;
        }
        else
        {
            if(!MemDC)
                return false;
            
            POINT OldPos;
            Status = MoveToEx(MemDC, StartX, StartY, &OldPos);
            Status = LineTo(MemDC, EndX, EndY);
            Status = MoveToEx(MemDC, OldPos.x, OldPos.y, nullptr);
            if(!Status)
            {
                ErrorHandler("[GdiPP]  Failed to DrawLine");
                return false;
            }
            return true;
        }
    }

    bool DrawEllipse(const int X, const int Y, const int Width, const int Height)
    {
        if(!DoubleBuffered)
        {
            if(!ScreenDC)
                return false;
            return Ellipse(ScreenDC, X, Y, X + Width, Y + Height);
        }
        else
        {
            if(!ScreenDC)
                return false;
            
            return Ellipse(MemDC, X, Y, X + Width, Y + Height);
        }
    }

    // Draws any content in the double biffer
    // DoTransparentBlt (in, optional): Preforms a transparent blt  
    // TransColor (in, optional): Specifys color to make transparent in the transparent blt
    // ROP (in, optional): Specifys copy mode to use in the blt call
    bool DrawDoubleBuffer(const bool DoTransparentBlt = false, const COLORREF TransColor = RGB(0, 0, 0), const DWORD ROP = SRCCOPY)
    {
        if(!DoTransparentBlt)
        {
            if(!BitBlt(ScreenDC, 0, 0, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, MemDC, 0, 0, ROP))
            {
                ErrorHandler("[GdiPP]  Failed to BitBlt");
                return false;
            }
            return true;
        }
        else
        {
#ifndef NoDepends
            if(!TransparentBlt(ScreenDC, 0, 0, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, MemDC, 0, 0, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, RGB(0, 0, 0)))
            {
                ErrorHandler("[GdiPP]  Failed to TransparentBlt");
                return false;
            }
            return true;
#endif
        }

        return false;
    }

    // Clears different ways depending on ClearMode
    // ClearMode (in): Specifys how to clear dc
    // CearBrush (in, optional): Brush to clear with (only used with GDIPP_FILLRECT)
    // Note: It is the responsibility of the caller to free the brush 
    void Clear(const DWORD ClearMode, const HBRUSH ClearBrush = (HBRUSH)GetStockObject(BLACK_BRUSH))
    {
        if(ClearMode == GDIPP_FILLRECT)
        {
            if(ClearBrush)
            {
                if(!DoubleBuffered)
                    FillRect(ScreenDC, &ClientRect, ClearBrush);
                else
                    FillRect(MemDC, &ClientRect, ClearBrush);
            }
        }
        else if(ClearMode == GDIPP_INVALIDATE)
        {
            InvalidateRect(Wnd, NULL, TRUE);
            UpdateWindow(Wnd);
        }
        else if(ClearMode == GDIPP_REDRAW)
        {
            RedrawWindow(Wnd, NULL, NULL, RDW_ERASENOW | RDW_ERASE | RDW_INVALIDATE);
        }
    }

    HWND Wnd = NULL;
    HDC ScreenDC = NULL;
    RECT ClientRect = {};
    HDC MemDC = NULL;
    HBITMAP MemBM = NULL;
    HBITMAP OldBM = NULL;
    void(*ErrorHandler)(std::string) = LogError;
    
private:
    HPEN OldPen = NULL;
    HBRUSH OldBrush = NULL;
    bool DoubleBuffered = false;
};

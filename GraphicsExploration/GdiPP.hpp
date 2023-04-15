#include <windows.h>

#define NoDepends

#define GDIPP_FILLRECT 0x1
#define GDIPP_REDRAW 0x2
#define GDIPP_INVALIDATE 0x3

#define SafeReleaseDC(Wnd, DC) __pragma(warning(disable:6387)) if((DC != NULL && DC != INVALID_HANDLE_VALUE) && Wnd != NULL) { ReleaseDC(Wnd, DC); } __pragma(warning(default:6387))
#define SafeDeleteDC(DC) __pragma(warning(disable:6387)) if(DC != NULL && DC != INVALID_HANDLE_VALUE) { DeleteDC(DC); } __pragma(warning(default:6387))
#define SafeDeleteObject(Obj) __pragma(warning(disable:6387)) if(Obj != NULL && Obj != INVALID_HANDLE_VALUE) { DeleteObject(Obj); } __pragma(warning(default:6387))

// TODO: 
// 1. Fix all lines with TODO next to them 
// 2. add aa using ms algo 
//     2a. Create original bitmap for drawing
//     2b. Create temporary bitmap (2x, 4x or 8x) larger than the original bitmap
//     2c. Render your graphics to this large temporary bitmap 
//        (use any GDI method, pen or brush you like) 
//        but scale the graphics appropriately
//     2d. Draw this temporary bitmap on the original bitmap scaled 
//        (i.e. using StretchDIBits() method or any other you like), 
//        but call SetStretchBltMode(HALFTONE)
//        before this last step for the original DC 
//        (which holds the original bitmap), and after scaling restore it back
// 3. Maybe add shared pointer like structure to brush and pen class
//      
//


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
            SafeDeleteObject(Brush);
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
    int RefCount = 0;
};

class GdiPP
{
public:
    static void LogError(std::string ErrorMsg)
    {
        std::cout << ErrorMsg << std::endl;
    }

    GdiPP() // Default Ctor
    {
        this->~GdiPP();
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
        SelectObject(MemDC, OldBM); // i dont think this line is needed TODO
        SelectObject(MemDC, OldPen); // i dont think this line is needed TODO
        SelectObject(MemDC, OldBrush); // i dont think this line is needed TODO
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
                //if(OldPen)
                //{
                    OldPen = (HPEN)SelectObject(ScreenDC, NewPen); // i think this should always be getting set TODO
                //}

                //else
                //{
                //    OldPen = (HPEN)SelectObject(ScreenDC, NewPen); // i think this should always be getting set TODO
                //}

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
                //if(OldPen)
                //{
                    OldPen = (HPEN)SelectObject(MemDC, NewPen); // i think this should always be getting set TODO
                //}

                //else
                //{
                //    OldPen = (HPEN)SelectObject(MemDC, NewPen); // i think this should always be getting set TODO
                //}

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
                //if(OldBrush)
                //{
                    OldBrush = (HBRUSH)SelectObject(ScreenDC, NewBrush); // i think this should always be getting set TODO
                //}
                //else
                //{
                //    OldBrush = (HBRUSH)SelectObject(ScreenDC, NewBrush); // i think this should always be getting set TODO
                //}

                if(!OldBrush)
                {
                    ErrorHandler("[GdiPP]  Error creating solid brush");
                    return false;
                }
            }
        }
        else
        {
            if(MemDC)
            {
                //if(OldBrush)
                //{
                    OldBrush = (HBRUSH)SelectObject(MemDC, NewBrush); // i think this should always be getting set TODO
                //}
                //else
                //{
                //    OldBrush = (HBRUSH)SelectObject(MemDC, NewBrush); // i think this should always be getting set TODO
                //}

                if(!OldBrush)
                {
                    ErrorHandler("[GdiPP]  Error creating solid brush");
                    return false;
                }
            }
        }
        return true;
    }

    bool ChangeBitmap(const HBITMAP NewBitMap)
    {
        if (!DoubleBuffered)
        {
            if (ScreenDC)
            {
                //if(OldBrush)
                //{
                    OldBM = (HBITMAP)SelectObject(ScreenDC, NewBitMap); // i think this should always be getting set TODO
                //}
                //else
                //{
                //    OldBM = (HBITMAP)SelectObject(ScreenDC, NewBitMap); // i think this should always be getting set TODO
                //}

                if (!OldBM)
                {
                    ErrorHandler("[GdiPP]  Error creating solid brush");
                    return false;
                }
            }
        }
        else
        {
            if (MemDC)
            {
                //if(OldBrush)
                //{
                    OldBM = (HBITMAP)SelectObject(MemDC, NewBitMap); // i think this should always be getting set TODO
                //}
                //else
                //{
                //    OldBM = (HBITMAP)SelectObject(ScreenDC, NewBitMap); // i think this should always be getting set TODO
                //}

                if (!OldBM)
                {
                    ErrorHandler("[GdiPP]  Error creating solid brush");
                    return false;
                }
            }
        }
    }

    //Unfilled Shapes
    const bool DrawRectangle(const int X, const int Y, const int Width, const int Height)
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

    const bool DrawRectangle(const int X, const int Y, const int Width, const int Height, HPEN Line)
    {
        bool Status = false;

        this->ChangePen(Line);

        if (!DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            Status = Rectangle(ScreenDC, X, Y, X + Width, Y + Height);

            this->ChangePen(this->OldPen);

            return Status;
        }
        else
        {
            if (!MemDC)
                return false;

            Status = Rectangle(MemDC, X, Y, X + Width, Y + Height);

            this->ChangePen(this->OldPen);

            return Status;
        }
    }

    const bool DrawStringA(const int X, const int Y, const std::string &Text, const COLORREF TextColor, const int BkMode)
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

    const bool DrawStringW(const int X, const int Y, const std::wstring &Text, const COLORREF TextColor, const int BkMode)
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

    const bool DrawLine(const int StartX, const int StartY, const int EndX, const int EndY)
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

    const bool DrawLine(const int StartX, const int StartY, const int EndX, const int EndY, HPEN Line)
    {
        bool Status = false;

        this->ChangePen(Line);

        if (!DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            POINT OldPos;
            Status = MoveToEx(ScreenDC, StartX, StartY, &OldPos);
            Status = LineTo(ScreenDC, EndX, EndY);
            Status = MoveToEx(ScreenDC, OldPos.x, OldPos.y, nullptr);
            if (!Status)
            {
                ErrorHandler("[GdiPP]  Failed to DrawLine");
                return false;
            }
            this->ChangePen(this->OldPen);
            return Status;
        }
        else
        {
            if (!MemDC)
                return false;

            POINT OldPos;
            Status = MoveToEx(MemDC, StartX, StartY, &OldPos);
            Status = LineTo(MemDC, EndX, EndY);
            Status = MoveToEx(MemDC, OldPos.x, OldPos.y, nullptr);
            if (!Status)
            {
                ErrorHandler("[GdiPP]  Failed to DrawLine");
                return false;
            }
            this->ChangePen(this->OldPen);
            return Status;
        }
    }

    const bool DrawEllipse(const int X, const int Y, const int Width, const int Height)
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

    const bool DrawEllipse(const int X, const int Y, const int Width, const int Height, HPEN Line)
    {
        bool Status = false;

        this->ChangePen(Line);

        if (!DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            Status = Ellipse(ScreenDC, X, Y, X + Width, Y + Height);

            this->ChangePen(this->OldPen);

            return Status;
        }
        else
        {
            if (!ScreenDC)
                return false;

            Status = Ellipse(MemDC, X, Y, X + Width, Y + Height);

            this->ChangePen(this->OldPen);

            return Status;
        }
    }

    const bool DrawTriangle(const int X1, const int Y1, const int X2, const int Y2, const int X3, const int Y3)
    {
        bool Status = false;

        Status = DrawLine(X1, Y1, X2, Y2);
        Status = DrawLine(X2, Y2, X3, Y3);
        Status = DrawLine(X3, Y3, X1, Y1);

        return Status;
    }

    const bool DrawTriangle(const int X1, const int Y1, const int X2, const int Y2, const int X3, const int Y3, HPEN Line)
    {
        bool Status = false;

        this->ChangePen(Line);

        Status = DrawLine(X1, Y1, X2, Y2);
        Status = DrawLine(X2, Y2, X3, Y3);
        Status = DrawLine(X3, Y3, X1, Y1);

        this->ChangePen(this->OldPen);

        return Status;
    }

    // Filled Shapes
    // Note: It is the responsibility of the caller to free the brush 
    const bool DrawFilledRect(const int X, const int Y, const int Width, const int Height, HBRUSH BG, HPEN OutLine = (HPEN)INVALID_HANDLE_VALUE)
    {
        if (!DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            RECT r = {};

            SetRect(&r, X, Y, X + Width, Y + Height);

            return FillRect(ScreenDC, &r, BG);
        }
        else
        {
            if (!MemDC)
                return false;

            RECT r = {};

            SetRect(&r, X, Y, X + Width, Y + Height);

            return FillRect(MemDC, &r, BG);
        }
    }

    const bool DrawFilledRect(const int X, const int Y, const int Width, const int Height)
    {
        if (!DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            // Top Left, Bottom Left, Bottom Right, Top Right
            POINT Verts[4] = { {X, Y}, {X, Y + Height}, {X+Width, Y+Height}, {X + Width, Y} };

            return Polygon(ScreenDC, Verts, 4);
        }
        else
        {
            if (!MemDC)
                return false;

            POINT Verts[4] = { {X, Y}, {X, Y + Height}, {X + Width, Y + Height}, {X + Width, Y} };

            return Polygon(MemDC, Verts, 4);
        }
    }

    const bool DrawFilledRect(const int X, const int Y, const int Width, const int Height, HPEN OutLine, HBRUSH BG = (HBRUSH)INVALID_HANDLE_VALUE)
    {
        bool Result = false;

        if (BG != (HBRUSH)INVALID_HANDLE_VALUE)
            this->ChangeBrush(BG);

        this->ChangePen(OutLine);

        if (!DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            // Top Left, Bottom Left, Bottom Right, Top Right
            POINT Verts[4] = { {X, Y}, {X, Y + Height}, {X + Width, Y + Height}, {X + Width, Y} };

            Result = Polygon(ScreenDC, Verts, 4);

            this->ChangePen(this->OldPen);
            if (BG != (HBRUSH)INVALID_HANDLE_VALUE)
            {
                this->ChangeBrush(this->OldBrush);
            }

            return Result;
        }
        else
        {
            if (!MemDC)
                return false;

            POINT Verts[4] = { {X, Y}, {X, Y + Height}, {X + Width, Y + Height}, {X + Width, Y} };

            Result =  Polygon(MemDC, Verts, 4);

            this->ChangePen(this->OldPen);
            if (BG != (HBRUSH)INVALID_HANDLE_VALUE)
            {
                this->ChangeBrush(this->OldBrush);
            }

            return Result;
        }
    }

    const bool DrawFilledTriangle(const int X1, const int Y1, const int X2, const int Y2, const int X3, const int Y3, HBRUSH BG = nullptr, HPEN OutLine = nullptr)
    {
        bool Result = false;

        if (OutLine != nullptr && OutLine != INVALID_HANDLE_VALUE)
        {
            this->ChangePen(OutLine);
        }

        if (BG != nullptr && BG != INVALID_HANDLE_VALUE)
        {
            this->ChangeBrush(BG);
        }

        if (!this->DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            POINT Verts[3] = { {X1, Y1}, {X2, Y2}, {X3, Y3} };

            Result = Polygon(ScreenDC, Verts, 3);
        }
        else
        {
            if (!MemDC)
                return false;

            POINT Verts[3] = { {X1, Y1}, {X2, Y2}, {X3, Y3} };

            Result = Polygon(MemDC, Verts, 3);
        }

        if (OutLine != nullptr)
        {
            this->ChangePen(this->OldPen);
        }

        if (BG != nullptr)
        {
            this->ChangeBrush(this->OldBrush);
        }

        return Result;
    }

    /*
    const bool DrawFilledTriangle(const int X1, const int Y1, const int X2, const int Y2, const int X3, const int Y3)
    {
        POINT Verts[3];
        Verts[0] = { X1, Y1 };
        Verts[1] = { X2, Y2 };
        Verts[2] = { X3, Y3 };

        if (!this->DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            return Polygon(ScreenDC, Verts, sizeof(Verts) / sizeof(Verts[0]));
        }
        else
        {
            if (!MemDC)
                return false;

            return Polygon(MemDC, Verts, sizeof(Verts) / sizeof(Verts[0]));
        }
    }
    */

    // Draws a polygon using the vertices
    // Vertices (In): Specifys the vertices to be used  
    // Returns true if successful
    const bool DrawPolygon(POINT Vertices[], int Count)
    {
        if (!this->DoubleBuffered)
        {
            if (!ScreenDC)
                return false;

            return Polygon(ScreenDC, Vertices, Count);
        }
        else
        {
            if (!MemDC)
                return false;

            return Polygon(MemDC, Vertices, Count);
        }
    }

    // Draws any content in the double biffer
    // DoTransparentBlt (in, optional): Preforms a transparent blt  
    // TransColor (in, optional): Specifys color to make transparent in the transparent blt
    // ROP (in, optional): Specifys copy mode to use in the blt call
    const bool DrawDoubleBuffer(const bool DoTransparentBlt = false, const COLORREF TransColor = RGB(0, 0, 0), const DWORD ROP = SRCCOPY)
    {
        if(!DoTransparentBlt)
        {
            GetClientRect(this->Wnd, &ClientRect);

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
            GetClientRect(this->Wnd, &ClientRect);
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
                GetClientRect(this->Wnd, &ClientRect);

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

    bool UpdateClientRgn()
    {
        return GetClientRect(Wnd, &this->ClientRect);
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

#include <iostream>
#include <string>
#include "GdiPP.hpp"
#include "WndCreator.hpp"
#include "TerraGL.h"

int sx = GetSystemMetrics(SM_CXSCREEN);
int sy = GetSystemMetrics(SM_CYSCREEN);

/*
void CustomErr(std::string Str)
{
    MessageBoxA(0, Str.c_str(), "Error!", MB_OK);
}

int main()
{
    WndCreator Cmd = GetConsoleWindow();
    Cmd.Hide();

    // Create Brush and Pen
    HBRUSH ClearBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

    // Create Overlay
    WndCreatorW Window = WndCreatorW(CS_OWNDC, L"MouseESP", L"Mouse Esp", LoadCursorW(NULL, IDC_ARROW), NULL, ClearBrush, 0, WndModes::BorderLess | WndModes::ClipChildren, 0, 0, sx, sy);

    //Window.SetLayeredAttributes(RGB(0, 0, 0), 0, LWA_COLORKEY);

    // Create gdi Object
    GdiPP gdi = GdiPP(Window.Wnd, true);
    gdi.ErrorHandler = CustomErr;

    PenPP CurrentPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

    // Select Pen And Brushb
    gdi.ChangeBrush(ClearBrush);
    gdi.ChangePen(CurrentPen);

    // Init Variables
    MSG msg = { 0 };
    POINT p;
    Window.CreateChildWindow(0, WS_CHILD | WS_VISIBLE, L"Button", L"Ok", 20, 20, 50, 20, (HMENU)201);

    int sx = Window.GetClientArea().Width;
    int sy = Window.GetClientArea().Height;

    while (!GetAsyncKeyState(VK_RETURN))
    {
        PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);
        // Translate and Dispatch message to WindowProc
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Check Msg
        if (msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY)
        {
            break;
        }

        if (GetAsyncKeyState(VK_F10))
        {
            CurrentPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
            gdi.ChangePen(CurrentPen);
            gdi.ChangeBrush(ClearBrush);
        }

        GetCursorPos(&p);
        // Do Drawing
        gdi.Clear(GDIPP_FILLRECT);
        std::string Str = "Mouse (" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")";
        gdi.DrawRectangle(p.x - 10, p.y - 10, 20, 20);
        gdi.DrawStringA(p.x - 10, p.y - 25, Str, RGB(255, 0, 0), TRANSPARENT);
        gdi.DrawLine(sx / 2, sy / 2, p.x, p.y);
        gdi.DrawDoubleBuffer();
    }

    Window.Destroy();

    return 0;
}*/

int main()
{
    WndCreator Cmd = GetConsoleWindow();
    Cmd.Hide();

    // Create Brush and Pen
    HBRUSH ClearBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

    // Create Overlay
    WndCreatorW Window = WndCreatorW(CS_OWNDC, L"MouseESP", L"Mouse Esp", LoadCursorW(NULL, IDC_ARROW), NULL, ClearBrush, 0, WndModes::BorderLess, 0, 0, sx, sy);

    //Window.SetLayeredAttributes(RGB(0, 0, 0), 0, LWA_COLORKEY);

    TerraGL Tgl = TerraGL(Window.Wnd);

    // Init Variables
    MSG msg = { 0 };
    POINT p;

    int sx = Window.GetClientArea().Width;
    int sy = Window.GetClientArea().Height;

    while (!GetAsyncKeyState(VK_RETURN))
    {
        PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);
        // Translate and Dispatch message to WindowProc
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Check Msg
        if (msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY)
        {
            break;
        }

        if (GetAsyncKeyState(VK_F10))
        {
            Tgl.SetBrushColor(D2D1::ColorF::Green);
        }

        GetCursorPos(&p);
        std::wstring Str = L"Mouse (" + std::to_wstring(p.x) + L", " + std::to_wstring(p.y) + L")";

        // Do Drawing
        Tgl.BeginDraw();
        Tgl.Clear();
        Tgl.RenderTarget->DrawRectangle(D2D1::RectF(p.x - 10, p.y - 10, p.x + 20, p.y + 20), Tgl.Brush, 2.0f);
        Tgl.RenderTarget->DrawTextW(Str.c_str(), Str.length(), Tgl.TextFormat, CalcCenterAlignTextLayout(p.x - 10, p.y - 25, 60, 20), Tgl.Brush);
        //gdi.DrawStringA(p.x - 10, p.y - 25, Str, RGB(255, 0, 0), TRANSPARENT);
        Tgl.RenderTarget->DrawLine(D2D1::Point2F(sx / 2, sy / 2), D2D1::Point2F(p.x, p.y), Tgl.Brush);
        Tgl.EndDraw();
    }

    Window.Destroy();

    return 0;
}
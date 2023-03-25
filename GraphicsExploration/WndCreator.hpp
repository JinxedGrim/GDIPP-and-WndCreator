#include <windows.h>
#include <iostream>

#define SafeReleaseDC(Wnd, DC) __pragma(warning(disable:6387)) if((DC != NULL && DC != INVALID_HANDLE_VALUE) && Wnd != NULL) { ReleaseDC(Wnd, DC); } __pragma(warning(default:6387))
#define SafeDeleteDC(DC) __pragma(warning(disable:6387)) if(DC != NULL && DC != INVALID_HANDLE_VALUE) { DeleteDC(DC); } __pragma(warning(default:6387))
#define SafeDeleteObject(Obj) __pragma(warning(disable:6387)) if(Obj != NULL && Obj != INVALID_HANDLE_VALUE) { DeleteObject(Obj); } __pragma(warning(default:6387))
#define SafeDestroyWindow(Wnd) __pragma(warning(disable:6387)) if(Wnd != NULL) { DestroyWindow(Wnd); } __pragma(warning(default:6387))

#ifdef UNICODE
#define WndCreator WndCreatorW
#else
#define WndCreator WndCreatorA
#endif // UNICODE


LRESULT CALLBACK WindowProcA(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY: // called when DestroyWindow is called
        {
            break; // calls DefWindowProcW: which will call WM_QUIT
        }
        case WM_CLOSE: // called when user clicks x button or alt f4
        {
            break; // calls DefWindowProcW: which will call Destroy Window 
        }
        case WM_QUIT: // closes window
        {
            break;
        }
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowProcW(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY: // called when DestroyWindow is called
        {
            break; // calls DefWindowProcW: which will call WM_QUIT
        }
        case WM_CLOSE: // called when user clicks x button or alt f4
        {
            break; // calls DefWindowProcW: which will call Destroy Window 
        }
        case WM_QUIT: // closes window
        {
            break;
        }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

class WndCreatorA
{
public:
    WndCreatorA()
    {
        if (!DidCreate)
        {
            wc = {};
            DidCreate = false;
            Wnd = NULL;
            ErrorHandler = LogError;
        }
        else
        {
            this->~WndCreatorA();
        }
    }

    WndCreatorA(const UINT ClassStyle, const HINSTANCE hInstance, const std::string ClassName, const HBRUSH BackGround, const DWORD ExFlags, const DWORD WStyle, const int x, const int y, const int Width, const int Height, WNDPROC WndProc = WindowProcA)
    {
        SecureZeroMemory(&wc, sizeof(WNDCLASSEXA));
        wc = { sizeof(WNDCLASSEXA), ClassStyle, WndProc, 0L, 0L, hInstance, NULL, NULL, BackGround, ClassName.c_str(), ClassName.c_str(), NULL };

        if (!RegisterClassExA(&wc))
        {
            ErrorHandler("[WndCreator] Failed To Register Window Class: " + GetLastError());
            return;
        }

        this->Wnd = CreateWindowExA(ExFlags, ClassName.c_str(), ClassName.c_str(), WStyle, x, y, Width, Height, 0, 0, hInstance, 0);

        if (!this->Wnd)
        {
            SafeDeleteObject(wc.hbrBackground);
            UnregisterClassA(wc.lpszClassName, wc.hInstance);
            ErrorHandler("[WndCreator] Failed To Create Window: " + GetLastError());
            return;
        }

        this->DidCreate = true;
    }

    WndCreatorA(const HWND Wnd)
    {
        this->Wnd = Wnd;

        this->DidCreate = false;
    }

    // Dtor
    ~WndCreatorA()
    {
        if (this->DidCreate)
        {
            SafeDestroyWindow(Wnd);
            SafeDeleteObject(wc.hbrBackground);
            UnregisterClassA(wc.lpszClassName, wc.hInstance);
        }
    }

    // Default Error Handler
    static void LogError(std::string ErrorMsg)
    {
        std::cout << ErrorMsg << std::endl;
    }

    // Copying is not allowed
    WndCreatorA(const WndCreatorA&) = delete;
    WndCreatorA& operator=(const WndCreatorA&) = delete;

    // Moving is allowed
    WndCreatorA(WndCreatorA&&) = default;
    WndCreatorA& operator=(WndCreatorA&&) = default;

private:
    WNDCLASSEXA wc = {};
    bool DidCreate = false;
public:

    HWND Wnd = NULL;
    void(*ErrorHandler)(std::string) = LogError;

    const bool Hide()
    {
        if (!this->Wnd)
        {
            return false;
        }

        return ShowWindow(this->Wnd, SW_HIDE);
    }

    const bool Show()
    {
        if (!this->Wnd)
        {
            return false;
        }

        return ShowWindow(this->Wnd, SW_SHOW);
    }

    const bool Maximize()
    {
        if (!this->Wnd)
        {
            return false;
        }

        return ShowWindow(this->Wnd, SW_MAXIMIZE);
    }

    const bool Minimize()
    {
        if (!this->Wnd)
        {
            return false;
        }

        return ShowWindow(this->Wnd, SW_MINIMIZE);
    }

    const bool AddStyleFlags(const DWORD StyleToAdd)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrA(this->Wnd, GWL_STYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style |= StyleToAdd;

        if (!SetWindowLongPtrA(this->Wnd, GWL_STYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const bool SubStyleFlags(const LONG_PTR StyleToSub)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrA(this->Wnd, GWL_STYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style &= ~(StyleToSub);
        
        if (!SetWindowLongPtrA(this->Wnd, GWL_STYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }
        
        Show();

        return true;
    }

    const bool ResetStyle(const LONG_PTR NewStyle)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrA(this->Wnd, GWL_STYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style = NewStyle;

        if (!SetWindowLongPtrA(this->Wnd, GWL_STYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const bool AddExStyleFlags(const LONG_PTR ExStyle)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrA(this->Wnd, GWL_EXSTYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style |= ExStyle;

        if (!SetWindowLongPtrA(this->Wnd, GWL_EXSTYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const bool SubExStyleFlags(const LONG_PTR ExStyle)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrA(this->Wnd, GWL_EXSTYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style &= ~(ExStyle);
        
        if (!SetWindowLongPtrA(this->Wnd, GWL_EXSTYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }
        
        Show();

        return true;
    }

    const bool ResetExStyle(const DWORD NewExStyle)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrA(this->Wnd, GWL_EXSTYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();
        Style = NewExStyle;

        if (!SetWindowLongPtrA(this->Wnd, GWL_EXSTYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const POINT GetWindowSz()
    {
        RECT rect;
        if (GetWindowRect(this->Wnd, &rect))
        {
            return{ rect.right - rect.left, rect.bottom - rect.top };
        }
        else
        {
            return {0, 0};
        }
    }

    const bool SetWndSz(const HWND WndZPos, const int X, int const Y, const int Width, const int Height, const UINT SwFlags)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        if (!SetWindowPos(this->Wnd, WndZPos, X, Y, Width, Height, SwFlags))
        {
            ErrorHandler("[WndCreator] Error with SetWindowPos: " + GetLastError());
            return false;
        }

        return true;
    }

    const bool SetLayeredAttributes(const COLORREF CrKey, const BYTE Alpha, const DWORD DwFlags = LWA_COLORKEY)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }
        
        if (!SetLayeredWindowAttributes(Wnd, CrKey, Alpha, DwFlags))
        {
            ErrorHandler("[WndCreator] Failed to set layered attributes: " + GetLastError());
            return false;
        }

        return true;
    }

    const LRESULT SendWndMessage(const UINT MSG, WPARAM WParam, LPARAM LParam)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return -1;
        }

        return SendMessageA(this->Wnd, MSG, WParam, LParam);
    }

    const void Destroy()
    {
        SafeDestroyWindow(this->Wnd);
    }
};

class WndCreatorW
{
public:
    WndCreatorW()
    {
        if (!DidCreate)
        {
            wc = {};
            DidCreate = false;
            Wnd = NULL;
            ErrorHandler = LogError;
        }
        else
        {
            this->~WndCreatorW();
        }
    }

    WndCreatorW(const UINT ClassStyle, const HINSTANCE hInstance, const std::wstring ClassName, const HBRUSH BackGround, const DWORD ExFlags, const DWORD WStyle, const int x, const int y, const int Width, const int Height, WNDPROC WndProc = WindowProcW)
    {
        SecureZeroMemory(&wc, sizeof(WNDCLASSEXW));
        wc = { sizeof(WNDCLASSEXW), ClassStyle, WndProc, 0L, 0L, hInstance, NULL, NULL, BackGround, ClassName.c_str(), ClassName.c_str(), NULL };

        if (!RegisterClassExW(&wc))
        {
            ErrorHandler("[WndCreator] Failed To Register Window Class: " + GetLastError());
            return;
        }

        this->Wnd = CreateWindowExW(ExFlags, ClassName.c_str(), ClassName.c_str(), WStyle, x, y, Width, Height, 0, 0, hInstance, 0);

        if (!this->Wnd)
        {
            SafeDeleteObject(wc.hbrBackground);
            UnregisterClassW(wc.lpszClassName, wc.hInstance);
            ErrorHandler("[WndCreator] Failed To Create Window: " + GetLastError());
            return;
        }

        this->DidCreate = true;
    }

    WndCreatorW(const HWND Wnd)
    {
        this->Wnd = Wnd;

        this->DidCreate = false;
    }

    ~WndCreatorW()
    {
        if (this->DidCreate)
        {
            SafeDestroyWindow(Wnd);
            SafeDeleteObject(wc.hbrBackground);
            UnregisterClassW(wc.lpszClassName, wc.hInstance);
        }
    }

    static void LogError(std::string ErrorMsg)
    {
        std::cout << ErrorMsg << std::endl;
    }

    // Copying is not allowed
    WndCreatorW(const WndCreatorW&) = delete;
    WndCreatorW& operator=(const WndCreatorW&) = delete;

    // Moving is allowed
    WndCreatorW(WndCreatorW&&) = default;
    WndCreatorW& operator=(WndCreatorW&&) = default;

private:
    WNDCLASSEXW wc = {};
    bool DidCreate = false;
public:
    HWND Wnd = NULL;
    void(*ErrorHandler)(std::string) = LogError;

    const bool Hide()
    {
        if (!this->Wnd)
        {
            return false;
        }

        return ShowWindow(this->Wnd, SW_HIDE);
    }

    const bool Show()
    {
        if (!this->Wnd)
        {
            return false;
        }

        return ShowWindow(this->Wnd, SW_SHOW);
    }

    const bool Maximize()
    {
        if (!this->Wnd)
        {
            return false;
        }

        return ShowWindow(this->Wnd, SW_MAXIMIZE);
    }

    const bool Minimize()
    {
        if (!this->Wnd)
        {
            return false;
        }

        return ShowWindow(this->Wnd, SW_MINIMIZE);
    }

    const bool AddStyleFlags(const DWORD StyleToAdd)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrW(this->Wnd, GWL_STYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style |= StyleToAdd;

        if (!SetWindowLongPtrW(this->Wnd, GWL_STYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const bool SubStyleFlags(const LONG_PTR StyleToSub)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrW(this->Wnd, GWL_STYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();
        
        Style &= ~(StyleToSub);

        if (!SetWindowLongPtrW(this->Wnd, GWL_STYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const bool ResetStyle(const LONG_PTR NewStyle)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrW(this->Wnd, GWL_STYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style = NewStyle;

        if (!SetWindowLongPtrW(this->Wnd, GWL_STYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const bool AddExStyleFlags(const LONG_PTR ExStyle)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrW(this->Wnd, GWL_EXSTYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style |= ExStyle;

        if (!SetWindowLongPtrW(this->Wnd, GWL_EXSTYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const bool SubExStyleFlags(const LONG_PTR ExStyle)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrW(this->Wnd, GWL_EXSTYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style &= ~(ExStyle);

        if (!SetWindowLongPtrW(this->Wnd, GWL_EXSTYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }
     
    const bool ResetExStyle(const DWORD NewExStyle)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        LONG_PTR Style = GetWindowLongPtrW(this->Wnd, GWL_EXSTYLE);

        if (!Style)
        {
            ErrorHandler("[WndCreator] Error Retrieving Window Style: " + GetLastError());
            return false;
        }

        Hide();

        Style = NewExStyle;

        if (!SetWindowLongPtrW(this->Wnd, GWL_EXSTYLE, Style))
        {
            ErrorHandler("[WndCreator] Error Setting Window Style: " + GetLastError());
            return false;
        }

        Show();

        return true;
    }

    const POINT GetWindowSz()
    {
        RECT rect;
        if (GetWindowRect(this->Wnd, &rect))
        {
            return{ rect.right - rect.left, rect.bottom - rect.top };
        }
        else
        {
            return { 0, 0 };
        }
    }

    const bool SetWndSz(const HWND WndZPos, const int X, const int Y, const int Width, const int Height, const UINT SwFlags)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        if (!SetWindowPos(this->Wnd, WndZPos, X, Y, Width, Height, SwFlags))
        {
            ErrorHandler("[WndCreator] Error with SetWindowPos: " + GetLastError());
            return false;
        }

        return true;
    }

    const bool SetLayeredAttributes(const COLORREF CrKey, const BYTE Alpha, const DWORD DwFlags = LWA_COLORKEY)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return false;
        }

        if (!SetLayeredWindowAttributes(Wnd, CrKey, Alpha, DwFlags))
        {
            ErrorHandler("[WndCreator] Failed to set layered attributes: " + GetLastError());
            return false;
        }

        return true;
    }
     
    const LRESULT SendWndMessage(const UINT MSG, WPARAM WParam, LPARAM LParam)
    {
        if (!this->Wnd)
        {
            ErrorHandler("[WndCreator] Error Window Handle Is Null");
            return -1;
        }

        return SendMessageW(this->Wnd, MSG, WParam, LParam);
    }

    const void Destroy()
    {
        SafeDestroyWindow(this->Wnd);
    }
};
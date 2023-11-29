#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

// Global Declarations
ID2D1Factory* pFactory = NULL;
ID2D1HwndRenderTarget* pRenderTarget = NULL;
ID2D1Bitmap* pBitmap = NULL;
ID2D1SolidColorBrush* pBrush = NULL;

class TerraGL
{
    public:

    HWND Hwnd = NULL;
    HDC ScreenDC = NULL;
    RECT ClientRect = {};
    D2D1_SIZE_U ScreenSz = {};

    ID2D1HwndRenderTarget* RenderTarget;
    ID2D1Factory* D2D1Factory;
    D2D1_BITMAP_PROPERTIES BitmapProperties;

    bool CurrentlyDrawing = false;
    BYTE* PixelBuffer = nullptr;

    bool UpdateClientRgn(bool UpdatePixelBuffer = false)
    {
        bool Stat = GetClientRect(this->Hwnd, &this->ClientRect);

        this->ScreenSz = D2D1::SizeU(ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top);

        HRESULT Hr = S_OK;

        Hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(this->Hwnd, this->ScreenSz), &RenderTarget);

        if (UpdatePixelBuffer == true)
        {
            if (PixelBuffer != nullptr)
                delete[] PixelBuffer;

            PixelBuffer = new BYTE[(ClientRect.right - ClientRect.left) * (ClientRect.bottom - ClientRect.top)];
        }

        return Stat == true && SUCCEEDED(Hr);
    }

    void SetBitmapProperties(const D2D1_PIXEL_FORMAT& PixelFormat, float DpiX = 96.0f, float DpiY = 96.0f)
    {
        this->BitmapProperties = D2D1::BitmapProperties(PixelFormat, DpiX, DpiY);
    }

    TerraGL(HWND _Wnd)
    {
        this->Hwnd = _Wnd;
        this->CurrentlyDrawing = false;

        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2D1Factory);
        this->UpdateClientRgn();

        this->SetBitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_STRAIGHT));
    }

    HRESULT DrawPixelBuffer()
    {
        HRESULT Hr = S_OK;

        if (SUCCEEDED(Hr))
        {
            Hr = pRenderTarget->CreateBitmap(this->ScreenSz, this->PixelBuffer,
                bufferWidth * 4, // Assuming 4 bytes per pixel (32 bits)
                &bitmapProperties,
                &pBitmap);
        }
    }

    __inline void BeginDraw()
    {
        this->CurrentlyDrawing = true;
        this->RenderTarget->BeginDraw();
    }

    __inline void Clear()
    {
        if (this->CurrentlyDrawing == false)
            this->BeginDraw();

        const D2D1_COLOR_F* ClearColor = (const D2D1_COLOR_F*)0;

        if (this->CurrentlyDrawing == false)
            this->EndDraw();
    }

    __inline void EndDraw()
    {
        this->RenderTarget->EndDraw();
        this->CurrentlyDrawing = false;
    }
};

// Function to release Direct2D resources
void DiscardGraphicsResources()
{
    if (pBitmap) pBitmap->Release();
    if (pRenderTarget) pRenderTarget->Release();
    if (pFactory) pFactory->Release();
}

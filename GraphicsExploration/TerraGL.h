#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>


#ifdef _MSC_VER
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")
#endif

class TerraGL
{
    public:

    HWND Hwnd = NULL;

    RECT ClientRect = {};
    D2D1_SIZE_U ScreenSz = {};

    ID2D1Factory* D2D1Factory;

    ID2D1HwndRenderTarget* RenderTarget;
    bool CurrentlyDrawing = false;

    D2D1_BITMAP_PROPERTIES BitmapProperties;
    ID2D1Bitmap* Bitmap;
    BYTE* PixelBuffer = nullptr;
    int Stride = 0;
    int PixelBufferWidth = 0;
    int PixelBufferHeight = 0;

    bool UpdateClientRgn(bool UpdatePixelBuffer = false)
    {
        bool Stat = GetClientRect(this->Hwnd, &this->ClientRect);

        this->ScreenSz = D2D1::SizeU(ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top);

        HRESULT Hr = S_OK;

        Hr = D2D1Factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(this->Hwnd, this->ScreenSz), &RenderTarget);

        if (UpdatePixelBuffer == true)
        {
            this->SetupPixelBuffer((ClientRect.right - ClientRect.left), (ClientRect.bottom - ClientRect.top), 4);
        }

        return Stat == true && SUCCEEDED(Hr);
    }

    void SetBitmapProperties(const D2D1_PIXEL_FORMAT& PixelFormat, float DpiX = 96.0f, float DpiY = 96.0f)
    {
        this->BitmapProperties = D2D1::BitmapProperties(PixelFormat, DpiX, DpiY);
    }

    void SetupPixelBuffer(int Width, int Height, int BytesPerPixel)
    {
        this->PixelBufferWidth = Width;
        this->PixelBufferHeight = Height;

        if (PixelBuffer != nullptr)
            delete[] PixelBuffer;

        this->Stride = ((this->PixelBufferWidth * BytesPerPixel + 3) / 4) * 4; // Calculate stride

        PixelBuffer = new BYTE[this->Stride * this->PixelBufferHeight];
    }

    TerraGL(HWND _Wnd)
    {
        this->Hwnd = _Wnd;
        this->CurrentlyDrawing = false;

        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2D1Factory);
        this->UpdateClientRgn(true);

        this->SetBitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_STRAIGHT));
    }

    bool DrawPixelBuffer()
    {
        HRESULT Hr = S_OK;

        if (SUCCEEDED(Hr))
        {
            Hr = this->RenderTarget->CreateBitmap(this->ScreenSz, (void*)this->PixelBuffer, (UINT32)this->Stride, this->BitmapProperties, &Bitmap);
        }

        return SUCCEEDED(Hr);
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

    ~TerraGL()
    {
        if(this->Bitmap)
        {
            Bitmap->Release();
        }

        if(this->PixelBuffer)
        {
            delete[] PixelBuffer;
        }

        if(this->RenderTarget)
        {
            RenderTarget->Release();
        }

        if(this->D2D1Factory)
        {
            D2D1Factory->Release();
        }
    }
};

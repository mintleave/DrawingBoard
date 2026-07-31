#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int xPos, yPos;     // 记录鼠标坐标
int xPoint0, yPoint0, xPoint1, yPoint1; // 绘制起点与终点
bool IsPointingLine = false, IsPointingRect = false;    // 是否处于绘制状态, 矩形还是线段

ID2D1Factory*          pFactory;
ID2D1HwndRenderTarget* pRenderTarget;
ID2D1SolidColorBrush*  pBrush;

// 创建 D2D 资源
HRESULT CreateGraphicsResources(HWND hwnd)
{
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd, size),
            &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            const D2D1_COLOR_F color = D2D1::ColorF(0,0,256);
            hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
        }
    }
    return hr;
}

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
// 释放 D2D 资源
void DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 白色背景
    wc.hCursor = LoadCursor(NULL, IDC_CROSS); // 十字准星光标

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }

    // 启动时创建pFactory
    case WM_CREATE:
    {
        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        return 0;
    }

    case WM_PAINT:
    {
        /*PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        // 用背景色填充整个客户区（防止残留）
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // 格式化数字
        // wchar_t buffer[64];
        // wsprintf(buffer, L"MOUSE: (%d, %d)", xPos, yPos);

        // 设置文字颜色
        SetTextColor(hdc, RGB(0, 0, 255));  // 蓝色
        SetBkMode(hdc, TRANSPARENT);        // 透明背景

        // 在窗口中央绘制
        // DrawText(hdc, buffer, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // All painting occurs here, between BeginPaint and EndPaint.
        // FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);*/

        HRESULT hr = CreateGraphicsResources(hwnd);
        if (SUCCEEDED(hr))
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

            pRenderTarget->BeginDraw();


            /*// 清屏（白色背景）
            pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

            // 画一条蓝色线，从 (50, 50) 到 (300, 200)
            pRenderTarget->DrawLine(
                D2D1::Point2F(50.0f, 50.0f),
                D2D1::Point2F(300.0f, 200.0f),
                pBrush,
                3.0f  // 线宽
            );*/

            pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

            if (IsPointingLine) {
                pRenderTarget->DrawLine(
                    D2D1::Point2F(xPoint0, yPoint0),
                    D2D1::Point2F(xPoint1, yPoint1),
                    pBrush,
                    3.0f  // 线宽
                );
            }
            else if (IsPointingRect) {
                pRenderTarget->DrawRectangle(
                    D2D1::RectF(xPoint0, yPoint0, xPoint1, yPoint1),
                    pBrush,
                    3.0f);
            }


            hr = pRenderTarget->EndDraw();
            if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
            {
                DiscardGraphicsResources();
            }
            EndPaint(hwnd, &ps);
        }

        return 0;
    }

    // 按下左键
    case WM_LBUTTONDOWN:
    {
        /*xPos = GET_X_LPARAM(lParam);
        yPos = GET_Y_LPARAM(lParam);
        InvalidateRect(hwnd, NULL, TRUE);*/  // 触发重绘

        xPoint0 = GET_X_LPARAM(lParam);
        yPoint0 = GET_Y_LPARAM(lParam);

        IsPointingLine = true;
        IsPointingRect = false;

        return 0;
    }

    // 松开左键
    case WM_LBUTTONUP:
    {
        /*xPos = GET_X_LPARAM(lParam);
        yPos = GET_Y_LPARAM(lParam);
        InvalidateRect(hwnd, NULL, TRUE);*/  // 触发重绘

        IsPointingLine = false;

        return 0;
    }

    // 按下右键
    case WM_RBUTTONDOWN:
    {
        xPoint0 = GET_X_LPARAM(lParam);
        yPoint0 = GET_Y_LPARAM(lParam);
        
        IsPointingLine = false;
        IsPointingRect = true;

        return 0;
    }

    // 松开右键
    case WM_RBUTTONUP:
    {
        IsPointingRect = false;
        return 0;
    }

    case WM_MOUSEMOVE: 
    {
        /*xPos = GET_X_LPARAM(lParam);
        yPos = GET_Y_LPARAM(lParam);
        InvalidateRect(hwnd, NULL, TRUE);*/  // 触发重绘

        if (IsPointingLine || IsPointingRect) {
            xPoint1 = GET_X_LPARAM(lParam);
            yPoint1 = GET_Y_LPARAM(lParam);
            InvalidateRect(hwnd, NULL, FALSE);
        }

        return 0;
    }

    case WM_SIZE:
    {
        /*// 窗口大小改变时，强制重绘整个窗口
        InvalidateRect(hwnd, NULL, TRUE);*/

        if (pRenderTarget != NULL)
        {
            RECT rc;
            GetClientRect(hwnd, &rc);

            D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

            pRenderTarget->Resize(size);
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


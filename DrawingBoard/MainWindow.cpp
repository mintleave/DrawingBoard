#include "MainWindow.h"

#include <windowsx.h>

MainWindow::MainWindow() : m_hwnd(NULL), m_pFactory(nullptr), m_pRenderTarget(nullptr), m_pBrush(nullptr) {

}

MainWindow::~MainWindow() {

}

// 创建 D2D 资源
HRESULT MainWindow::CreateGraphicsResources() {
    HRESULT hr = S_OK;
    if (m_pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = m_pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget);

        if (SUCCEEDED(hr))
        {
            const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 256);
            hr = m_pRenderTarget->CreateSolidColorBrush(color, &m_pBrush);
        }
    }
    return hr;
}

template <class T> static void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
// 释放 D2D 资源
void MainWindow::DiscardGraphicsResources() {
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBrush);
}

BOOL MainWindow::Create(
    PCWSTR lpWindowName,
    DWORD dwStyle,
    DWORD dwExStyle,
    int x,
    int y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu
)
{
    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ClassName();
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 白色背景
    wc.hCursor = LoadCursor(NULL, IDC_CROSS); // 十字准星光标

    RegisterClass(&wc); // 注册

    m_hwnd = CreateWindowEx(
        0,                              //  扩展窗口样式
        ClassName(),                     //  窗口类名称
        lpWindowName,                    // 窗口标题
        dwStyle,                        // 窗口样式

        // x坐标，y坐标，宽度，高度
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // 父窗口句柄    
        NULL,       // 菜单句柄
        GetModuleHandle(NULL),  // 实例句柄
        this    // 注意: 必须传this!
    );

    return (m_hwnd ? TRUE : FALSE);
}


LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* pThis = NULL;

    if (uMsg == WM_NCCREATE)
    {
        // 从 CREATESTRUCT 中提取 this 指针
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pCreate->lpCreateParams;

        // 保存到窗口的用户数据中
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

        // 保存窗口句柄到对象
        pThis->m_hwnd = hwnd;
    }
    else
    {
        // 从窗口的用户数据中取出 this 指针
        pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        // 调用成员函数！
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        // 启动时创建m_pFactory
        case WM_CREATE:
        {
            if (FAILED(D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory)))
            {
                return -1; 
            }
            return 0;
        }

        case WM_PAINT:
        {
            HRESULT hr = CreateGraphicsResources();
            if (SUCCEEDED(hr))
            {
                PAINTSTRUCT ps;
                BeginPaint(m_hwnd, &ps);

                m_pRenderTarget->BeginDraw();

                m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

                // 绘制保存的线段
                for (auto points : m_PointedLine) {
                    m_pRenderTarget->DrawLine(
                        points.first,
                        points.second,
                        m_pBrush,
                        3.0f 
                    );
                }
                // 绘制保存的矩形
                for (auto points : m_PointedRect) {
                    m_pRenderTarget->DrawRectangle(
                        D2D1::RectF(points.first.x, points.first.y, points.second.x, points.second.y),
                        m_pBrush,
                        3.0f);
                }

                // 实时渲染
                if (IsPointingLine) {
                   m_pRenderTarget->DrawLine(
                        D2D1::Point2F(xPoint0, yPoint0),
                        D2D1::Point2F(xPoint1, yPoint1),
                        m_pBrush,
                        3.0f
                    );
                }
                else if (IsPointingRect) {
                    m_pRenderTarget->DrawRectangle(
                        D2D1::RectF(xPoint0, yPoint0, xPoint1, yPoint1),
                        m_pBrush,
                        3.0f);
                }


                hr = m_pRenderTarget->EndDraw();
                if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
                {
                    DiscardGraphicsResources();
                }
                EndPaint(m_hwnd, &ps);
            }

            return 0;
        }

        // 按下左键
        case WM_LBUTTONDOWN:
        {
            xPoint0 = GET_X_LPARAM(lParam);
            yPoint0 = GET_Y_LPARAM(lParam);

            IsPointingLine = true;
            IsPointingRect = false;

            return 0;
        }

        // 松开左键
        case WM_LBUTTONUP:
        {
            D2D1_POINT_2F point0(xPoint0, yPoint0), point1(xPoint1, yPoint1);
            if (!(point0.x == point1.x && point0.y == point1.y)) {
                m_PointedLine.emplace_back(point0, point1);
            }
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
            D2D1_POINT_2F point0(xPoint0, yPoint0), point1(xPoint1, yPoint1);
            if (!(point0.x == point1.x && point0.y == point1.y)) {
                m_PointedRect.emplace_back(point0, point1);
            }
            IsPointingRect = false;
            return 0;
        }

        // 鼠标移动
        case WM_MOUSEMOVE:
        {
            if (IsPointingLine || IsPointingRect) {
                xPoint1 = GET_X_LPARAM(lParam);
                yPoint1 = GET_Y_LPARAM(lParam);
                InvalidateRect(m_hwnd, NULL, FALSE);
            }
        }

        // 改变大小
        case WM_SIZE:
        {
            if (m_pRenderTarget != NULL)
            {
                RECT rc;
                GetClientRect(m_hwnd, &rc);

                D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

                m_pRenderTarget->Resize(size);
                InvalidateRect(m_hwnd, NULL, FALSE);
            }
        }

        return 0;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

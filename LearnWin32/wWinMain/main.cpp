#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int xPos, yPos; // 记录鼠标坐标

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
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        // 用背景色填充整个客户区（防止残留）
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // 格式化数字
        wchar_t buffer[64];
        wsprintf(buffer, L"MOUSE: (%d, %d)", xPos, yPos);

        // 设置文字颜色
        SetTextColor(hdc, RGB(0, 0, 255));  // 蓝色
        SetBkMode(hdc, TRANSPARENT);        // 透明背景

        // 在窗口中央绘制
        DrawText(hdc, buffer, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // All painting occurs here, between BeginPaint and EndPaint.
        // FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        xPos = GET_X_LPARAM(lParam);
        yPos = GET_Y_LPARAM(lParam);
        InvalidateRect(hwnd, NULL, TRUE);  // 触发重绘
        return 0;
    }

    case WM_LBUTTONUP:
    {
        xPos = GET_X_LPARAM(lParam);
        yPos = GET_Y_LPARAM(lParam);
        InvalidateRect(hwnd, NULL, TRUE);  // 触发重绘
        return 0;
    }

    case WM_MOUSEMOVE: 
    {
        xPos = GET_X_LPARAM(lParam);
        yPos = GET_Y_LPARAM(lParam);
        InvalidateRect(hwnd, NULL, TRUE);  // 触发重绘
        return 0;
    }

    case WM_SIZE:
    {
        // 窗口大小改变时，强制重绘整个窗口
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


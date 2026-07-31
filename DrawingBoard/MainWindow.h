#pragma once

#include <windows.h>
#include <d2d1.h>
#include <vector>
#pragma comment(lib, "d2d1")

// 参考: https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/begin/LearnWin32/BaseWindow/cpp/main.cpp
class MainWindow
{
public:

    MainWindow();
    ~MainWindow();

    // 创建窗口
    BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    );

    // 非静态会传入this指针, 静态如何调用成员?
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // 消息处理
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND Window() const { return m_hwnd; }

private:
    HWND m_hwnd;

    ID2D1Factory* m_pFactory;                   // 用于创建D2D资源
    ID2D1HwndRenderTarget* m_pRenderTarget;     // 绑定到窗口（HWND）的渲染目标, 相当于画布
    ID2D1SolidColorBrush* m_pBrush;             // 画刷，用来指定绘制时用的颜色

    int xPoint0, yPoint0, xPoint1, yPoint1; // 绘制起点与终点
    bool IsPointingLine = false, IsPointingRect = false;    // 是否处于绘制状态, 矩形还是线段

    std::vector<std::pair<D2D1_POINT_2F, D2D1_POINT_2F>> m_PointedLine, m_PointedRect; // 存储已经绘制的图形

	PCWSTR  ClassName() const { return L"DrawingBoard"; }   // 窗口类名称

    // 创建 D2D 资源
    HRESULT CreateGraphicsResources();
    // 释放 D2D 资源
    void DiscardGraphicsResources();
};


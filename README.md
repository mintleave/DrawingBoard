# DrawingBoard
win32的窗口项目，实现一个简单的画板，要求实现绘制线段、矩形功能

## 需求分析 

1. 创建窗口

2. 鼠标输入

3. 图形绘制

# LearnWin32

## wWinMain

[案例来源](https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/begin/LearnWin32/HelloWorld/cpp/main.cpp)

![](./img/wWinMain.png)

### 窗口类

``` c++
const wchar_t CLASS_NAME[]  = L"Sample Window Class";

WNDCLASS wc = { };

// WindowProc 将定义大部分行为 (如鼠标点击、键盘输入、重绘、关闭)
wc.lpfnWndProc   = WindowProc;
// 用于标记当前窗口属于那个进程
wc.hInstance     = hInstance;
// 命名窗口
wc.lpszClassName = CLASS_NAME;
```

### 创建窗口

``` c++
HWND hwnd = CreateWindowEx(
    0,                              //  扩展窗口样式
    CLASS_NAME,                     //  窗口类名称
    L"Learn to Program Windows",    // 窗口标题
    WS_OVERLAPPEDWINDOW,            // 窗口样式

    // x坐标，y坐标，宽度，高度
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

    NULL,       // 父窗口句柄    
    NULL,       // 菜单句柄
    hInstance,  // 实例句柄
    NULL        // 额外创建数据
    );

if (hwnd == NULL)
{
    return 0;
}
```


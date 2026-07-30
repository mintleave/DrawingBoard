# DrawingBoard
win32的窗口项目，实现一个简单的画板，要求实现绘制线段、矩形功能

## 需求分析 

1. 创建窗口

2. 鼠标输入

3. 图形绘制

# LearnWin32

## 创建窗口

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

## 消息处理

处理鼠标点击和移动消息，将坐标打印到屏幕中间。
![](./img/MouseMove.png)

### 窗口消息

用户按下鼠标左键，窗口将收到一条消息，消息代码如下。

``` c++
#define WM_LBUTTONDOWN    0x0201
```

### 消息循环

对于创建窗口的每个线程，操作系统都会为窗口消息创建队列。可以通过调用 GetMessage 函数从队列拉取消息。

``` c++
MSG msg;
GetMessage(&msg, NULL, 0, 0);
```

此函数从队列的头中删除第一条消息。 如果队列为空，该函数将阻塞，直到另一条消息进入队列。

尽管 MSG 结构包含有关消息的信息，但您几乎永远不会直接检查此结构。 而是将它直接传递给另外两个函数。

``` c++
TranslateMessage(&msg); 
DispatchMessage(&msg);
```

TranslateMessage 函数与键盘输入相关。 它将击键（按下按键，松开按键）转换为字符。 您不必了解此函数的工作原理；只需记得在 DispatchMessage 之前调用它即可。

DispatchMessage 函数告诉操作系统调用消息目标窗口的窗口过程。 换句话说，操作系统会在其窗口表中查找窗口句柄，找到与窗口关联的函数指针，并调用该函数。

例如，假设用户按下鼠标左键。 这会引发一连串事件：

1. 操作系统在消息队列上放置 WM_LBUTTONDOWN 消息。
2. 程序调用 GetMessage 函数。
3. GetMessage 从队列中提取 WM_LBUTTONDOWN 消息，并填写 MSG 结构。
4. 程序调用 TranslateMessage 和 DispatchMessage 函数。
5. 在 DispatchMessag 中，操作系统调用窗口过程。
6. 窗口过程可以响应消息或忽略它。

### 编写窗口过程

DispatchMessage 函数调用窗口的窗口过程，该窗口是消息的目标。 窗口过程具有以下签名。

``` c++
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
```

通常有四个参数。

- hwnd 是窗口的句柄。
- uMsg 是消息代码；例如，WM_SIZE 消息指示窗口已调整大小。
- wParam 和 lParam 包含与消息相关的其他数据。 具体含义依赖于消息代码。

## 鼠标输入

### 响应鼠标单击

如果用户在光标位于窗口工作区上时单击鼠标按钮，该窗口将收到以下消息之一。

| 消息                |  含义
|-----------          | --- 
|WM_LBUTTONDOWN       | 按下左键
|WM_LBUTTONUP         | 松开左键
|WM_MBUTTONDOWN       | 按下中键
|WM_MBUTTONUP         | 松开中键
|WM_RBUTTONDOWN       | 按下右键
|WM_RBUTTONUP         | 松开右键

### 鼠标坐标

在所有这些消息中，lParam 参数包含鼠标指针的 x 坐标和 y 坐标。

``` c++
int xPos = GET_X_LPARAM(lParam); 
int yPos = GET_Y_LPARAM(lParam);
```

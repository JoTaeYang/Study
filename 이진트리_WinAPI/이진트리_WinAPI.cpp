// 이진트리_WinAPI.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "이진트리_WinAPI.h"
#include "CBinaryTree.h"

HINSTANCE g_Inst;
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

CBinaryTree g_Tree;
bool g_init = false;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // TODO: 여기에 코드를 입력합니다.

    g_Inst = hInstance;

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPI));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_WINAPI);
    wcex.lpszClassName = L"BasicWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(L"BasicWindowClass", L"BASE", WS_OVERLAPPED |
        WS_CAPTION |
        WS_SYSMENU |
        WS_THICKFRAME |
        WS_MINIMIZEBOX |
        WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        return FALSE;
    }



    g_Tree.InsertNode(100);
    g_Tree.InsertNode(50);
    g_Tree.InsertNode(150);
    g_Tree.InsertNode(40);
    g_Tree.InsertNode(70);
    g_Tree.InsertNode(110);
    g_Tree.InsertNode(105);
    g_Tree.InsertNode(103);
    g_Tree.InsertNode(160);
    g_Tree.InsertNode(170);
    g_Tree.InsertNode(120);
    g_Tree.InsertNode(115);
    g_Tree.InsertNode(117);

    ShowWindow(hWnd, nCmdShow);

    UpdateWindow(hWnd);

    MSG msg;
    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int _x;
    static int _y;
    static BOOL _linedraw = FALSE;
    switch (message)
    {
    case WM_CREATE:
    {
        g_init = true;
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if(g_init == true)
            g_Tree.GDIPrint(hWnd);
        //Ellipse(hdc, 150, 0, 200, 50);
        //TextOut(hdc, 160, 20, L"Hello", 5);
        //Ellipse(hdc, 100, 50, 150, 100);
        //Ellipse(hdc, 200, 50, 250, 100);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
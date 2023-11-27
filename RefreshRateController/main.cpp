#include <Windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <iostream>

#include "resource.h"

#define MYWM_NOTIFYICON (WM_APP + 1)

NOTIFYICONDATA nid;

HWND ghwnd;

void update_refresh_rate(DWORD desired_frequency)
{
    DEVMODE dm;
    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);
    if (0 != EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
    {
        dm.dmDisplayFrequency = desired_frequency;
        LONG result = ChangeDisplaySettingsEx(NULL, &dm, NULL, 0, NULL);
        if (result != DISP_CHANGE_SUCCESSFUL)
            MessageBox(ghwnd, L"Failed to update refres rate", L"Error", MB_ICONINFORMATION);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ghwnd = hwnd;
    switch (uMsg)
    {
    case MYWM_NOTIFYICON:
        switch (LOWORD(lParam))
        {
        case WM_RBUTTONUP:
        {
            HMENU hPopupMenu = CreatePopupMenu();
            AppendMenu(hPopupMenu, MF_STRING, 1, L"60Hz");
            AppendMenu(hPopupMenu, MF_STRING, 2, L"144Hz");
            AppendMenu(hPopupMenu, MF_STRING, 3, L"Exit");

            POINT pt;
            GetCursorPos(&pt);

            SetForegroundWindow(hwnd);
            TrackPopupMenu(hPopupMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            PostMessage(hwnd, WM_NULL, 0, 0);
            DestroyMenu(hPopupMenu);
        }
        break;

        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            update_refresh_rate(60);
            break;
        case 2:
            update_refresh_rate(144);
            break;
        case 3:
            exit(0);
            break;
        }
        break;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main()
{
    FreeConsole();

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"Refresh Rate Controller";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, L"Refresh Rate Controller", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);

    HICON hCustomIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = MYWM_NOTIFYICON;
    nid.hIcon = hCustomIcon;
    wcscpy_s(nid.szTip, L"Refresh Rate Controller");

    Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

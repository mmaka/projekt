#pragma once

#define MYMENU_EXIT         (WM_APP + 101)
#define MYMENU_MESSAGEBOX   (WM_APP + 102)


HINSTANCE  inj_hModule;
HWND       prnt_hWnd;

BOOL RegisterDLLWindowClass(wchar_t szClassName[]);
HMENU CreateDLLWindowMenu();
DWORD WINAPI ThreadProc(LPVOID lpParam);
LRESULT CALLBACK DLLWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
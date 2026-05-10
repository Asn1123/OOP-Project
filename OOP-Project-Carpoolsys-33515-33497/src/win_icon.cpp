#ifdef _WIN32
#include <windows.h>

void setAppIcon(void* hwnd) {
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1));
    SendMessage((HWND)hwnd, WM_SETICON, ICON_BIG,   (LPARAM)hIcon);
    SendMessage((HWND)hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}
#endif

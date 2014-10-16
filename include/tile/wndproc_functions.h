
#ifndef WNDPROC_FUNCTIONS_H
#define WNDPROC_FUNCTIONS_H

#include "./common_headers.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToastWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK BorderWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK scan(HWND, LPARAM);
void paint_statusline(HWND const&);

#endif

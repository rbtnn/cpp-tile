
#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include "./common_headers.h"

RECT get_window_area();
void die(std::string const&);
WNDCLASSEX make_wndclassex(HINSTANCE, LRESULT CALLBACK (*)(HWND, UINT, WPARAM, LPARAM), std::string);
HWND make_toolwindow(HINSTANCE, std::string);
std::string get_classname(HWND const);
std::string get_windowtext(HWND const);
bool exist_file(std::string const);
void resize_window(HWND const&, HWND const&, int const&, unsigned int, unsigned int, unsigned int, unsigned int);
void system_error(std::string const&);
bool is_manageable(HWND const&);

#endif


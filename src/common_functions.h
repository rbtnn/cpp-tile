
#include "./common_headers.h"

long get_statusline_height();
RECT get_window_area();
long get_statusline_width();
void die(std::string const&);
WNDCLASSEX make_wndclassex(HINSTANCE, LRESULT CALLBACK (*)(HWND, UINT, WPARAM, LPARAM), std::string);
HWND make_toolwindow(HINSTANCE, std::string);
std::string get_classname(HWND const);
std::string get_windowtext(HWND const);
bool exist_file(std::string const);
void resize_window(RECT const&, HWND const&, unsigned int, unsigned int, unsigned int, unsigned int);

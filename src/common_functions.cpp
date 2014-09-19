
#include "./common_headers.h"

long get_statusline_height(){
  return 40;
}
RECT get_window_area(){
  RECT wa;
  HWND const hwnd = ::FindWindow("Shell_TrayWnd", NULL);
  if (hwnd && ::IsWindowVisible(hwnd)) {
    ::SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
  }
  else {
    wa.left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    wa.top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    wa.right = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    wa.bottom = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
  }
  wa.top += get_statusline_height();
  return wa;
}
long get_statusline_width(){
  RECT rect = get_window_area();
  long const width = rect.right - rect.left;
  return width;
}
void die(std::string const& msg){
  std::cout << msg << std::endl;
  ::exit(EXIT_FAILURE);
}
WNDCLASSEX make_wndclassex(HINSTANCE hInstance_, LRESULT CALLBACK (* wndproc_)(HWND, UINT, WPARAM, LPARAM), std::string classname_){
  WNDCLASSEX wincls;
  wincls.cbSize = sizeof(::WNDCLASSEX);
  wincls.style = 0;
  wincls.lpfnWndProc = wndproc_;
  wincls.cbClsExtra = 0;
  wincls.cbWndExtra = 0;
  wincls.hInstance = hInstance_;
  wincls.hIcon = NULL;
  wincls.hIconSm = NULL;
  wincls.hCursor = ::LoadCursor(NULL , IDC_ARROW);
  wincls.hbrBackground = NULL;
  wincls.lpszMenuName = NULL;
  wincls.lpszClassName = classname_.c_str();
  return wincls;
}
HWND make_toolwindow(HINSTANCE hInstance_, std::string classname_){
  HWND hwnd = ::CreateWindowEx(WS_EX_TOOLWINDOW,
      classname_.c_str(), classname_.c_str(),
      WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
      0, 0, 0, 0, NULL, NULL,
      hInstance_, NULL);
  if (!hwnd){
    die("Error creating window(" + classname_ + ")");
  }
  return hwnd;
}
std::string get_classname(HWND const hwnd_){
  char buffer[buffer_size];
  ::GetClassName(hwnd_, buffer, sizeof(buffer) / sizeof(char));
  return std::string(buffer);
}
std::string get_windowtext(HWND const hwnd_){
  char buffer[buffer_size];
  ::GetWindowText(hwnd_, buffer, sizeof(buffer) / sizeof(char));
  return std::string(buffer);
}

bool exist_file(std::string const path){
  WIN32_FIND_DATA findData;
  HANDLE hFindFile = ::FindFirstFile(path.c_str(), &findData);
  if(hFindFile != INVALID_HANDLE_VALUE){
    ::FindClose(hFindFile);
    return true;
  }
  else{
    return false;
  }
}
void resize_window(HWND const& hwnd_, HWND const& hWndInsertAfter_, int const& cmdshow_, unsigned int left_, unsigned int top_, unsigned int width_, unsigned int height_){
  RECT const window_area = get_window_area();
  ::ShowWindow(hwnd_, cmdshow_);

  RECT rect;
  ::GetWindowRect(hwnd_, &rect);
  bool const b = static_cast<unsigned int>(rect.right - rect.left) == width_
              && static_cast<unsigned int>(rect.bottom - rect.top) == height_
              && static_cast<unsigned int>(rect.top) == (get_statusline_height() + top_)
              && static_cast<unsigned int>(rect.left) == left_;
  if(!b){
    ::SetWindowPos(hwnd_, hWndInsertAfter_,
        window_area.left + left_,
        window_area.top + top_,
        width_,
        height_,
        SWP_NOACTIVATE);
  }

#ifdef DEBUG
  std::cout << "[resize_window]" << std::endl
    << ", w.left:" << window_area.left
    << ", w.top:" << window_area.top
    << ", left:" << left_
    << ", top:" << top_
    << ", width:" << width_
    << ", height:" << height_
    << std::endl
    << ", left:" << rect.left
    << ", top:" << rect.top
    << ", right:" << rect.right
    << ", bottom:" << rect.bottom
    << std::endl
    << b
    << std::endl
    << std::endl;
#endif
}


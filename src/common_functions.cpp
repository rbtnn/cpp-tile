
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
void resize_window(RECT const& window_area_, HWND const& hwnd_, unsigned int left_, unsigned int top_, unsigned int width_, unsigned int height_){
  ::ShowWindow(hwnd_, SW_SHOWNORMAL);
  ::SetWindowPos(hwnd_, HWND_TOP,
      window_area_.left + left_,
      window_area_.top + top_,
      width_,
      height_,
      SWP_NOACTIVATE);
  // std::cout << "hwnd:" << hwnd_
  //   << ", left:" << left_
  //   << ", top:" << top_
  //   << ", width:" << width_
  //   << ", height:" << height_
  //   << std::endl;
}


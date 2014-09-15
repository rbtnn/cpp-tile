
#include "./common_headers.h"
#include "./common_functions.h"
#include "./wndproc_functions.h"
#include "./tile/TilingWindowManager.h"
#include "./tile/Layout.h"

std::shared_ptr<Tile::TilingWindowManager> g_p_tile_window_manager(nullptr);

void arrange(std::deque<HWND> const& hwnds_){
  RECT window_area = get_window_area();
  long const width = window_area.right - window_area.left;
  long const height = window_area.bottom - window_area.top;
  long const split_size = 3;
  long sub_height = (2 < hwnds_.size()) ? (height / (hwnds_.size() - 1)) : height;
  for(unsigned int i = 0; i < hwnds_.size(); i++){
    resize_window(window_area, hwnds_.at(i),
        ((i == 0) ? 0                                                      : width / split_size * 2),
        ((i == 0) ? 0                                                      : sub_height * (i - 1)),
        ((i == 0) ? ( hwnds_.size() == 1 ? width : width / split_size * 2) : width / split_size),
        ((i == 0) ? height                                                 : sub_height));
  }
}
void arrange_twin(std::deque<HWND> const& hwnds_){
  RECT window_area = get_window_area();
  long const width = window_area.right - window_area.left;
  long const height = window_area.bottom - window_area.top;
  if(0 == hwnds_.size()){
    // nop
  }
  else if(1 == hwnds_.size()){
    resize_window(window_area, hwnds_.at(0), 0, 0, width, height);
  }
  else if(2 == hwnds_.size()){
    resize_window(window_area, hwnds_.at(0), 0, 0, width / 2        , height);
    resize_window(window_area, hwnds_.at(1), width / 2, 0, width / 2, height);
  }
  else{
    long const n = hwnds_.size() - 2;
    long const sub_width = width / n;

    resize_window(window_area, hwnds_.at(0), 0        , 0, width / 2, height / 4 * 3);
    resize_window(window_area, hwnds_.at(1), width / 2, 0, width / 2, height / 4 * 3);

    for(unsigned int i = 2; i < hwnds_.size(); i++){
      resize_window(window_area, hwnds_.at(i),
          (sub_width * (i - 2)),
          (height / 4 * 3),
          (sub_width),
          (height / 4 * 1));
    }
  }
}
void arrange_maximal(std::deque<HWND> const& hwnds_){
  RECT window_area = get_window_area();
  long const width = window_area.right - window_area.left;
  long const height = window_area.bottom - window_area.top;
  HWND const foreground_hwnd = ::GetForegroundWindow();
  for(auto hwnd : hwnds_){
    resize_window(window_area, ((hwnd == foreground_hwnd) ? foreground_hwnd : hwnd), 0, 0, width, height);
  }
}

int WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance_, LPSTR lpCmdLine_, int nShowCmd_){
#ifdef DEBUG
  bool const use_console = true;
  if(use_console){
    ::AllocConsole();
    ::freopen("CONOUT$", "w", stdout);
    ::freopen("CONIN$", "r", stdin);
  }
  else{
    ::freopen("./log.txt", "w", stdout);
  }
#endif
  // HMODULE h = ::LoadLibrary("test.dll");
  // if(h == NULL){
  //   ::MessageBox(NULL, "null", "", MB_OK);
  // }
  // else{
  //   typedef int (*TFUNC)(int);
  //   TFUNC DllFunction = (TFUNC)::GetProcAddress(h, "MyFunction");
  //   DllFunction(12);
  //   ::FreeLibrary(h);
  // }

  if(::FindWindow("Tile", NULL) != NULL){
    ::MessageBox(NULL, "Multiplex starting is not permitted.", "Error", MB_ICONERROR);
  }
  else{
    std::shared_ptr<Tile::ConfigReader> const configreader(new Tile::ConfigReader);

    g_p_tile_window_manager.reset(new Tile::TilingWindowManager(hInstance_, "Tile", {
          Tile::Layout("arrange", arrange),
          Tile::Layout("arrange_twin", arrange_twin),
          Tile::Layout("arrange_maximal", arrange_maximal),
          }, configreader));

    if(exist_file(configreader->get_inipath())){
      g_p_tile_window_manager->start();
    }
    else{
      ::MessageBox(NULL, ("'" + configreader->get_inipath() + "' does not exist.").c_str(), "Error", MB_ICONERROR);
    }
  }

#ifdef DEBUG
  if(use_console){
    ::FreeConsole();
  }
  else{
  }
#endif

  return 0;
}
LRESULT CALLBACK MainWndProc(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_){
  switch (msg_) {
    case WM_CLOSE:
    case WM_DESTROY:
      ::PostQuitMessage(0);
      return 0;

    case WM_HOTKEY:
      if(g_p_tile_window_manager != nullptr){
        g_p_tile_window_manager->call_key_method(wParam_);
        g_p_tile_window_manager->arrange();
      }
      break;

    default:
      if(g_p_tile_window_manager != nullptr){
        switch(wParam_ & 0x7fff){
          case HSHELL_WINDOWCREATED:
            g_p_tile_window_manager->manage((HWND)lParam_);
            g_p_tile_window_manager->arrange();
            break;

          case HSHELL_WINDOWDESTROYED:
            g_p_tile_window_manager->unmanage((HWND)lParam_);
            g_p_tile_window_manager->arrange();
            break;

          case HSHELL_WINDOWACTIVATED:
            g_p_tile_window_manager->manage((HWND)lParam_);
            g_p_tile_window_manager->arrange();
            break;
        }
      }
  }
  return ::DefWindowProc(hwnd_,msg_, wParam_, lParam_);
}
LRESULT CALLBACK StatusLineWndProc(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_){
  switch (msg_) {
    case WM_DESTROY:
      ::PostQuitMessage(0);
      break;

    case WM_CREATE:
      ::SetTimer(hwnd_, 1, 500, NULL);
      break;

    case WM_TIMER:
    case WM_PAINT:
      if(g_p_tile_window_manager != nullptr){
        PAINTSTRUCT ps;
        ::BeginPaint(hwnd_, &ps);
        HDC hdc = ::GetWindowDC(hwnd_);
        ::SelectObject(hdc, ::GetStockObject(BLACK_BRUSH));
        ::Rectangle(hdc , 0, 0, get_statusline_width(), get_statusline_height());
        RECT rect;
        rect.left = 10;
        rect.top = 10;
        rect.right = get_statusline_width();
        rect.bottom = get_statusline_height();
        ::SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
        ::SetBkColor(hdc, RGB(0x00, 0x00, 0x00));

        std::string const classname = get_classname(hwnd_);
        std::string const windowtext = get_windowtext(hwnd_);

        SYSTEMTIME stTime;
        ::GetLocalTime(&stTime);

        std::stringstream ss;
        ss << "[" << stTime.wYear << "/" << stTime.wMonth << "/" << stTime.wDay
          << " " << stTime.wHour << ":" << stTime.wMinute << ":" << stTime.wSecond << "]"
          << "[" << g_p_tile_window_manager->get_workspace_name() << "]"
          << "[" << g_p_tile_window_manager->get_managed_window_size() << "]"
          << "[" << g_p_tile_window_manager->get_layout_name() << "]"
          << "[" << classname << "] " << windowtext;

        ::DrawText(hdc, ss.str().c_str(), -1, &rect, DT_LEFT | DT_WORDBREAK);
        ::ReleaseDC(hwnd_, hdc);
        ::EndPaint(hwnd_, &ps);
      }
      break;
  }
  return ::DefWindowProc(hwnd_, msg_, wParam_, lParam_);
}
LRESULT CALLBACK BorderWndProc(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_){
  switch (msg_) {
    case WM_DESTROY:
      ::PostQuitMessage(0);
      break;

    case WM_CREATE:
      ::SetTimer(hwnd_, 1, 1000, NULL);
      break;

    case WM_PAINT:
      PAINTSTRUCT ps;
      ::BeginPaint(hwnd_, &ps);
      RECT rect;
      ::GetClientRect(hwnd_, &rect);
      HDC hdc = ::GetWindowDC(hwnd_);
      HBRUSH hbrush = ::CreateSolidBrush(RGB(255, 0, 0));
      ::FillRect(hdc , &rect, hbrush);
      ::ReleaseDC(hwnd_, hdc);
      ::EndPaint(hwnd_, &ps);
      break;
  }
  return ::DefWindowProc(hwnd_, msg_, wParam_, lParam_);
}
BOOL CALLBACK scan(HWND hwnd_, LPARAM lParam_){
  g_p_tile_window_manager->manage(hwnd_);
  if(hwnd_ != NULL){
    ::EnumChildWindows(hwnd_, scan, 0);
  }
  return TRUE;
}


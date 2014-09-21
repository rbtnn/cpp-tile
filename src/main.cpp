
#include "./common_headers.h"
#include "./common_functions.h"
#include "./wndproc_functions.h"
#include "./tile/TilingWindowManager.h"
#include "./tile/Layout.h"

std::shared_ptr<Tile::TilingWindowManager> g_p_tile_window_manager(nullptr);

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

  if(::FindWindow("Tile", NULL) != NULL){
    ::MessageBox(NULL, "Multiplex starting is not permitted.", "Error", MB_ICONERROR);
  }
  else{
    std::vector<HMODULE> module_handles;
    try{
      std::shared_ptr<Tile::ConfigReader> const configreader(new Tile::ConfigReader);

      std::vector<Tile::Layout> layouts;
      std::vector<std::string> const layout_names = configreader->get_layout_method_names();

      for(auto name : layout_names){
        HMODULE const h = ::LoadLibrary((name + ".dll").c_str());
        if(h != NULL){
          module_handles.push_back(h);
          Tile::Layout::ArrangeFuncRef const arrange_funcref = reinterpret_cast<Tile::Layout::ArrangeFuncRef>(::GetProcAddress(h, name.c_str()));
          if(arrange_funcref != NULL){
            layouts.push_back(Tile::Layout(name.c_str(), arrange_funcref));
          }
        }
      }

      g_p_tile_window_manager.reset(new Tile::TilingWindowManager(hInstance_, "Tile", layouts, configreader));
      g_p_tile_window_manager->start();
    }
    catch(std::exception const& e){
      ::MessageBox(NULL, e.what(), "Error", MB_ICONERROR);
      // ::MessageBox(NULL, ("'" + configreader->get_inifile_path() + "' does not exist.").c_str(), "Error", MB_ICONERROR);
    }
    for(auto h : module_handles){
      ::FreeLibrary(h);
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
    case WM_CREATE:
      ::SetTimer(hwnd_, 1, 1000, NULL);
      break;

    case WM_TIMER:
      if(g_p_tile_window_manager != nullptr){
        g_p_tile_window_manager->arrange();
      }
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      ::PostQuitMessage(0);
      return 0;

    case WM_HOTKEY:
      if(g_p_tile_window_manager != nullptr){
        g_p_tile_window_manager->call_key_method(wParam_);
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
      paint_statusline(hwnd_);
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

void paint_statusline(HWND const& hwnd_){
  PAINTSTRUCT ps;
  ::BeginPaint(hwnd_, &ps);

  HDC const hdc = ::GetWindowDC(hwnd_);
  HBITMAP const hBitmap = ::CreateCompatibleBitmap(hdc, get_statusline_width(), get_statusline_height());
  HDC const hdcMem = ::CreateCompatibleDC(hdc);
  ::SelectObject(hdcMem, hBitmap);

  if(g_p_tile_window_manager != nullptr){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    std::string const classname = get_classname(foreground_hwnd);
    std::string const windowtext = get_windowtext(foreground_hwnd);
    COLORREF const color = RGB(0x40, 0x40, 0x40);
    HBRUSH const hbrush = ::CreateSolidBrush(color);
    HFONT const hFont = ::CreateFont(16, 0, 0, 0,
        FW_REGULAR, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
        FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック");

    ::SelectObject(hdcMem, hbrush);
    ::Rectangle(hdcMem , 0, 0, get_statusline_width(), get_statusline_height());
    ::SetBkColor(hdcMem, color);

    ::SelectObject(hdcMem, hFont);

    RECT rect;
    rect.left = 3;
    rect.top = 3;
    rect.right = get_statusline_width();
    rect.bottom = get_statusline_height();

    ::SetTextColor(hdcMem, RGB(0x00, 0xff, 0x00));

    SYSTEMTIME stTime;
    ::GetLocalTime(&stTime);

    std::stringstream ss_time;
    ss_time
      << std::setfill('0') << std::setw(4) << stTime.wYear << "/"
      << std::setfill('0') << std::setw(2) << stTime.wMonth << "/"
      << std::setfill('0') << std::setw(2) << stTime.wDay << " "
      << std::setfill('0') << std::setw(2) << stTime.wHour << ":"
      << std::setfill('0') << std::setw(2) << stTime.wMinute << ":"
      << std::setfill('0') << std::setw(2) << stTime.wSecond;
    ::DrawText(hdcMem, ss_time.str().c_str(), -1, &rect, DT_LEFT | DT_WORDBREAK);

    rect.left = 160;

    SYSTEM_POWER_STATUS systemPowerStatus;
    ::GetSystemPowerStatus(&systemPowerStatus);

    std::stringstream ss_text;

    ss_text << "<<< "
      << "Workspace: " << g_p_tile_window_manager->get_workspace_name()
      << ", Managed: " << g_p_tile_window_manager->get_managed_window_size()
      << ", Layout: "  << g_p_tile_window_manager->get_layout_name()
      << ", Class: " << std::left << classname
      << ", Battery: " << std::left
      << static_cast<unsigned int>(systemPowerStatus.BatteryLifePercent) << "%";
    switch(systemPowerStatus.ACLineStatus){
      case 0: ss_text << "[Offline]"; break;
      case 1: ss_text << "[Online]"; break;
      case 255: ss_text << "[Unknown]"; break;
    }
    ss_text << " >>>";

    ::DrawText(hdcMem, ss_text.str().c_str(), -1, &rect, DT_LEFT | DT_WORDBREAK);

    rect.top = 20;
    rect.left = 3;

    ::SetTextColor(hdcMem, RGB(0xff, 0xff, 0x00));

    ::DrawText(hdcMem, windowtext.c_str(), -1, &rect, DT_LEFT | DT_WORDBREAK);
    ::ReleaseDC(hwnd_, hdcMem);
  }

  ::BitBlt(hdc, 0, 0, get_statusline_width(), get_statusline_height(), hdcMem, 0, 0, SRCCOPY);

  ::ReleaseDC(hwnd_, hdc);

  ::EndPaint(hwnd_, &ps);

  ::DeleteDC(hdc);
  ::DeleteDC(hdcMem);
  ::DeleteObject(hBitmap);
}


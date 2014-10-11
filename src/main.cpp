
#include <tile/common_headers.h>
#include <tile/common_functions.h>
#include <tile/wndproc_functions.h>
#include <tile/TilingWindowManager.h>
#include <tile/Layout.h>

std::shared_ptr<Tile::TilingWindowManager> g_p_tile_window_manager(nullptr);

int WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance_, LPSTR lpCmdLine_, int nShowCmd_){
#ifdef DEBUG
  bool const use_console = false;
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
    system_error("Multiplex starting is not permitted.");
  }
  else{
    std::vector<HMODULE> module_handles;
    try{
      std::shared_ptr<Tile::ConfigReader> const configreader(new Tile::ConfigReader(lpCmdLine_));
      std::shared_ptr<std::vector<Tile::Layout>> layouts(new std::vector<Tile::Layout>());

      auto const layout_names = configreader->get_layout_method_names();

      for(auto name : layout_names.value){
        HMODULE const h = ::LoadLibrary((name + ".dll").c_str());
        if(h != NULL){
          module_handles.push_back(h);
          Tile::Layout::ArrangeFuncRef const arrange_funcref = reinterpret_cast<Tile::Layout::ArrangeFuncRef>(::GetProcAddress(h, name.c_str()));
          if(arrange_funcref != NULL){
            layouts->push_back(Tile::Layout(name.c_str(), arrange_funcref));
          }
        }
      }

      g_p_tile_window_manager.reset(new Tile::TilingWindowManager(hInstance_, "Tile", layouts, configreader));
      g_p_tile_window_manager->start();
    }
    catch(std::exception const& e){
      system_error(e.what());
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
#ifdef DEBUG
      std::cout << "[WM_HOTKEY]" << std::endl;
      std::cout << "wParam:"  << wParam_ << std::endl;
      std::cout << "lParam:"  << lParam_ << std::endl;
#endif
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


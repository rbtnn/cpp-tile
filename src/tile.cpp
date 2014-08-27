
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT      0x0500

#include <windows.h>
#include <winuser.h>
#include <shellapi.h>

// #define DEBUG

#ifdef DEBUG
#  include <cstdio>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <algorithm>

#include <stdlib.h>
#include <time.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK scan(HWND, LPARAM);

namespace Tile{
  class TilingWindowManager;
}
std::shared_ptr<Tile::TilingWindowManager> g_p_tile_window_manager(nullptr);

void print_infomation_of(HWND const hwnd_){
  // HWND const parent = ::GetParent(hwnd_);
  // HWND const owner = ::GetWindow(hwnd_, GW_OWNER);
  char buffer[256];
  ::GetWindowText(hwnd_, buffer, sizeof(buffer) / sizeof(char));
  std::cout << "Hwnd:" << hwnd_ << std::endl;
  // std::cout << "ParentHwnd:" << parent << std::endl;
  // std::cout << "OwnerHwnd:" << owner << std::endl;
  std::cout << "Title:" << buffer << std::endl;
  // std::cout << "IsWindow:" << ::IsWindow(hwnd_) << std::endl;
  // std::cout << "Visible:" << ::IsWindowVisible(hwnd_) << std::endl;
  std::cout << std::endl;
}
bool is_manageable(HWND const hwnd_){
  if(hwnd_ == NULL){
    return false;
  }

  // if (g_twmp->window(hwnd_)){
  //   return true;
  // }
  // if(!g_twmp->window(parent)){
  //   if (pok){
  //     g_twmp->manage(parent);
  //   }
  // }

  HWND const parent = ::GetParent(hwnd_);
  int const style = ::GetWindowLong(hwnd_, GWL_STYLE);
  int const exstyle = ::GetWindowLong(hwnd_, GWL_EXSTYLE);
  bool const pok = (parent != 0 && is_manageable(parent));
  bool const istool = exstyle & WS_EX_TOOLWINDOW;
  bool const isapp = exstyle & WS_EX_APPWINDOW;

  if (::GetWindowTextLength(hwnd_) == 0) {
    return false;
  }

  if (style & WS_DISABLED) {
    return false;
  }

  if((parent == 0 && ::IsWindowVisible(hwnd_)) || pok){
    if((!istool && parent == 0) || (istool && pok)){
      return true;
    }
    if(isapp && parent != 0){
      return true;
    }
  }

  return false;
}
::RECT get_window_area(){
  ::RECT wa;
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
  return wa;
}

namespace Tile{
  void arrange1(std::vector<HWND> const& hwnds_){
    RECT const rect = get_window_area();
    long const width = (rect.right - rect.left) / 2;
    long const height = (rect.bottom - rect.top) / 2;
    int n = 0;
    for(auto hwnd : hwnds_){
      switch(n % 4){
        case 0:
          ::SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
          break;
        case 1:
          ::SetWindowPos(hwnd, HWND_TOP, rect.left + width, rect.top, width, height, SWP_NOACTIVATE);
          break;
        case 2:
          ::SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top + height, width, height, SWP_NOACTIVATE);
          break;
        case 3:
          ::SetWindowPos(hwnd, HWND_TOP, rect.left + width, rect.top + height, width, height, SWP_NOACTIVATE);
          break;
      }
      n++;
    }
  }

  namespace Wrapper{
    void die(std::string const& msg) {
      std::cout << msg << std::endl;
      ::exit(EXIT_FAILURE);
    }
    void RegisterShellHookWindow(HWND const& hwnd_){
      typedef BOOL (* RegisterShellHookWindowProc) (HWND);
      RegisterShellHookWindowProc p = (RegisterShellHookWindowProc) ::GetProcAddress( ::GetModuleHandle("USER32.DLL"), "RegisterShellHookWindow");
      if (!p){
        Tile::Wrapper::die("Could not find RegisterShellHookWindow");
      }
      p(hwnd_);
    }
  }

  class ConfigReader{
    private:
    public:
      ConfigReader(){
      }
      std::string get_path(){
        std::string const ini_filename = "tile.ini";
        char buffer[256];
        DWORD const rtn = ::GetEnvironmentVariable( "HOME", buffer, sizeof(buffer) / sizeof(char));
        if(rtn == 0){
          return ".\\" + ini_filename;
        }
        else{
          return std::string(buffer) + "\\" + ini_filename;
        }
      }
  };
  class Key{
    private:
      UINT const m_mod;
      UINT const m_k;
      UINT const m_hash_value;
      std::function<void(void)> const m_f;
      HWND const m_hwnd;

    public:
      Key(HWND const& hwnd_, UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_)
        : m_mod(mod_), m_k(k_), m_hash_value((m_mod * 100) + m_k), m_f(f_), m_hwnd(hwnd_) {
          ::RegisterHotKey(m_hwnd, m_hash_value, m_mod, m_k);
        }
      ~Key(){
        ::UnregisterHotKey(m_hwnd, m_hash_value);
      }
      UINT hash() const{
        return m_hash_value;
      }
      void call() const{
        m_f();
      }
  };
  class Layout{
    private:
      std::function<void(std::vector<HWND> const& hwnds_)> m_f;
    public:
      Layout(std::function<void(std::vector<HWND> const& hwnds_)> f_): m_f(f_){
      };
      void arrange(std::vector<HWND> const& hwnds_){
        m_f(hwnds_);
      };
  };
  class TilingWindowManager{
    private:
      std::string m_class_name;
      HWND m_hwnd;
      UINT m_shellhookid;
      HINSTANCE m_hInstance;
      std::shared_ptr<Tile::Layout> m_playout;
      std::vector<std::shared_ptr<Key> > m_keys;
      std::vector<HWND> m_managed_hwnds;

    public:
      TilingWindowManager(std::shared_ptr<Tile::Layout> playout_, const HINSTANCE& hInstance_){
        m_hInstance = hInstance_;
        m_class_name = "Tile";
        m_playout = playout_;

        ::WNDCLASSEX winClass;
        winClass.cbSize = sizeof(::WNDCLASSEX);
        winClass.style = 0;
        winClass.lpfnWndProc = WndProc;
        winClass.cbClsExtra = 0;
        winClass.cbWndExtra = 0;
        winClass.hInstance = hInstance_;
        winClass.hIcon = NULL;
        winClass.hIconSm = NULL;
        winClass.hCursor = NULL;
        winClass.hbrBackground = NULL;
        winClass.lpszMenuName = NULL;
        winClass.lpszClassName = m_class_name.c_str();
        ATOM atom = ::RegisterClassEx(&winClass);
        if ( ! atom){
          Tile::Wrapper::die("Error registering window class");
        }

        m_hwnd = ::CreateWindowEx(0,
            m_class_name.c_str(), m_class_name.c_str(),
            0, 0, 0, 0, 0, NULL, NULL,
            hInstance_, NULL);
        if ( ! m_hwnd){
          Wrapper::die("Error creating window");
        }

        m_shellhookid = ::RegisterWindowMessage("SHELLHOOK");
      };
      ~TilingWindowManager(){
        if ( ! m_hwnd){
          ::DestroyWindow(m_hwnd);
        }
        if ( ! m_hInstance){
          ::UnregisterClass(m_class_name.c_str(), m_hInstance);
        }
      }
      void arrange(){
        m_playout->arrange(m_managed_hwnds);
      };
      void manage(HWND hwnd_){
        if(is_manageable(hwnd_)){
          auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
          if(it == m_managed_hwnds.end()){
            m_managed_hwnds.push_back(hwnd_);
          }
        }
      }
      void unmanage(HWND hwnd_){
        auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
        if(it != m_managed_hwnds.end()){
          m_managed_hwnds.erase(it);
        }
      }
      void print_managed_windows(){
        for(auto hwnd : m_managed_hwnds){
          print_infomation_of(hwnd);
        }
      }
      UINT const start(){
        ::EnumWindows(scan, 0);

        Wrapper::RegisterShellHookWindow(m_hwnd);

        arrange();

        ::MSG msg;
        while (::GetMessage(&msg, NULL, 0, 0) > 0){
          ::TranslateMessage(&msg);
          ::DispatchMessage(&msg);
        }

        ::DeregisterShellHookWindow(m_hwnd);

        return msg.wParam;
      };
      void regist_key( UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_){
        std::shared_ptr<Key> p(new Key(m_hwnd, mod_, k_, f_));
        m_keys.push_back(p);
      };
      void call_key_method(UINT const& i_) const{
        for(auto key : m_keys){
          if(key->hash() == i_){
            key->call();
          }
        }
      };
      HWND const& get_hwnd() const{
        return m_hwnd;
      };
  };
}

void keyevent_quit(){
  ::PostMessage(g_p_tile_window_manager->get_hwnd(), WM_CLOSE, 0, 0);
}
void keyevent_print_managed_windows(){
  g_p_tile_window_manager->print_managed_windows();
}
void keyevent_arrange(){
  g_p_tile_window_manager->arrange();
}

int WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance_, LPSTR lpCmdLine_, int nShowCmd_){
#ifdef DEBUG
  ::AllocConsole();
  // ::freopen("./log.txt", "w", stdout);
  ::freopen("CONOUT$", "w", stdout);
  ::freopen("CONIN$", "r", stdin);
#endif

  g_p_tile_window_manager.reset(
      new Tile::TilingWindowManager(std::shared_ptr<Tile::Layout>(new Tile::Layout(Tile::arrange1)), hInstance_)
      );

  unsigned long const MODKEY = MOD_CONTROL | MOD_ALT;

  g_p_tile_window_manager->regist_key( MODKEY,            'Q',       keyevent_quit );
  g_p_tile_window_manager->regist_key( MODKEY,            'P',       keyevent_print_managed_windows );
  g_p_tile_window_manager->regist_key( MODKEY,            'A',       keyevent_arrange );

  g_p_tile_window_manager->start();

#ifdef DEBUG
  ::FreeConsole();
#endif

  return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_){
  if(g_p_tile_window_manager != nullptr){
    switch (msg_) {
      case WM_CLOSE:
      case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

      case WM_HOTKEY:
        g_p_tile_window_manager->call_key_method(wParam_);
        break;

      default:
        switch(wParam_ & 0x7fff){
          case HSHELL_WINDOWCREATED:
            g_p_tile_window_manager->manage((HWND)lParam_);
            break;

          case HSHELL_WINDOWDESTROYED:
            g_p_tile_window_manager->unmanage((HWND)lParam_);
            break;

          case HSHELL_WINDOWACTIVATED:
            g_p_tile_window_manager->manage((HWND)lParam_);
            break;

        }
    }
  }
  return ::DefWindowProc(hwnd_,msg_, wParam_, lParam_);
}
BOOL CALLBACK scan(HWND hwnd_, LPARAM lParam_){
  g_p_tile_window_manager->manage(hwnd_);
  if(hwnd_ != NULL){
    ::EnumChildWindows(hwnd_, scan, 0);
  }
  return TRUE;
}




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
#include <deque>
#include <map>
#include <functional>
#include <algorithm>

#include <stdlib.h>
#include <time.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK StatusLineWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK scan(HWND, LPARAM);

namespace Tile{
  void die(std::string const& msg) {
    std::cout << msg << std::endl;
    ::exit(EXIT_FAILURE);
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
    return wa;
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
      HWND const m_main_hwnd;

    public:
      Key(HWND const& hwnd_, UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_)
        : m_mod(mod_), m_k(k_), m_hash_value((m_mod * 100) + m_k), m_f(f_), m_main_hwnd(hwnd_) {
          ::RegisterHotKey(m_main_hwnd, m_hash_value, m_mod, m_k);
        }
      ~Key(){
        ::UnregisterHotKey(m_main_hwnd, m_hash_value);
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
      std::function<void(std::deque<HWND> const& hwnds_)> m_f;
    public:
      Layout(std::function<void(std::deque<HWND> const& hwnds_)> f_): m_f(f_){
      };
      void arrange(std::deque<HWND> const& hwnds_){
        m_f(hwnds_);
      };
  };
  class TilingWindowManager{
    private:
      HINSTANCE m_hInstance;

      HWND m_main_hwnd;
      std::string m_main_class_name;
      HWND m_statusline_hwnd;
      std::string m_statusline_class_name;

      UINT m_shellhookid;
      std::shared_ptr<Tile::Layout> m_playout;
      std::vector<std::shared_ptr<Key> > m_keys;
      std::deque<HWND> m_managed_hwnds;
      std::map<HWND, RECT> m_managed_hwnd_to_rect;
      std::map<HWND, LONG> m_managed_hwnd_to_style;

      void init_main(){
        m_main_class_name = "Tile";
        WNDCLASSEX winClass;
        winClass.cbSize = sizeof(::WNDCLASSEX);
        winClass.style = 0;
        winClass.lpfnWndProc = WndProc;
        winClass.cbClsExtra = 0;
        winClass.cbWndExtra = 0;
        winClass.hInstance = m_hInstance;
        winClass.hIcon = NULL;
        winClass.hIconSm = NULL;
        winClass.hCursor = NULL;
        winClass.hbrBackground = NULL;
        winClass.lpszMenuName = NULL;
        winClass.lpszClassName = m_main_class_name.c_str();
        ATOM atom = ::RegisterClassEx(&winClass);
        if (!atom){
          Tile::die("Error registering window class(2)");
        }

        m_main_hwnd = ::CreateWindowEx(0,
            m_main_class_name.c_str(), m_main_class_name.c_str(),
            0, 0, 0, 0, 0, NULL, NULL,
            m_hInstance, NULL);
        if (!m_main_hwnd){
          Tile::die("Error creating window(2)");
        }

      }
      void init_statusline(){
        m_statusline_class_name = "TileStatusLine";
        WNDCLASSEX winClass;
        winClass.cbSize = sizeof(::WNDCLASSEX);
        winClass.style = 0;
        winClass.lpfnWndProc = StatusLineWndProc;
        winClass.cbClsExtra = 0;
        winClass.cbWndExtra = 0;
        winClass.hInstance = m_hInstance;
        winClass.hIcon = NULL;
        winClass.hIconSm = NULL;
        winClass.hCursor = NULL;
        winClass.hbrBackground = NULL;
        winClass.lpszMenuName = NULL;
        winClass.lpszClassName = m_statusline_class_name.c_str();
        ATOM atom = ::RegisterClassEx(&winClass);
        if(!atom){
          Tile::die("Error registering window class(1)");
        }

        m_statusline_hwnd = ::CreateWindowEx(WS_EX_TOOLWINDOW,
            m_statusline_class_name.c_str(), m_statusline_class_name.c_str(),
            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, get_statusline_width(), get_statusline_height(), NULL, NULL,
            m_hInstance, NULL);
        if (!m_statusline_hwnd){
          Tile::die("Error creating window(2)");
        }
      }

      bool is_manageable(HWND const hwnd_){
        if(hwnd_ == NULL){
          return false;
        }

        HWND const parent = ::GetParent(hwnd_);
        int const style = ::GetWindowLong(hwnd_, GWL_STYLE);
        int const exstyle = ::GetWindowLong(hwnd_, GWL_EXSTYLE);
        bool const pok = (parent != 0 && is_manageable(parent));
        bool const istool = exstyle & WS_EX_TOOLWINDOW;
        bool const isapp = exstyle & WS_EX_APPWINDOW;

        char buffer[256];
        ::GetClassName(hwnd_, buffer, sizeof(buffer) / sizeof(char));

        if(std::string(buffer) == "ImmersiveLauncher"){ return false; }
        if(std::string(buffer) == "Shell_CharmWindow"){ return false; }
        if(std::string(buffer) == "ImmersiveBackgroundWindow"){ return false; }
        if(std::string(buffer) == "Snapped Desktop"){ return false; }
        if(std::string(buffer) == m_statusline_class_name){ return false; }

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

    public:
      TilingWindowManager(std::shared_ptr<Tile::Layout> playout_, const HINSTANCE& hInstance_){
        m_hInstance = hInstance_;
        m_playout = playout_;

        init_main();
        init_statusline();

        m_shellhookid = ::RegisterWindowMessage("SHELLHOOK");
      };
      ~TilingWindowManager(){
        for(auto hwnd : m_managed_hwnds){
          unmanage(hwnd);
        }
        if (!m_main_hwnd){
          ::DestroyWindow(m_main_hwnd);
        }
        if (!m_statusline_hwnd){
          ::DestroyWindow(m_statusline_hwnd);
        }
        if (!m_hInstance){
          ::UnregisterClass(m_statusline_class_name.c_str(), m_hInstance);
          ::UnregisterClass(m_main_class_name.c_str(), m_hInstance);
        }
      }

      void arrange(){
        m_playout->arrange(m_managed_hwnds);
      };
      void manage(HWND hwnd_){
        if(is_manageable(hwnd_)){
          auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
          if(it == m_managed_hwnds.end()){
            RECT rect;
            ::GetWindowRect(hwnd_, &rect);
            m_managed_hwnds.push_back(hwnd_);
            m_managed_hwnd_to_rect.insert(std::map<HWND, RECT>::value_type(hwnd_, rect));
            LONG const style = ::GetWindowLong(hwnd_, GWL_STYLE);
            m_managed_hwnd_to_style.insert(std::map<HWND, LONG>::value_type(hwnd_, style));

            char buffer[256];
            ::GetClassName(hwnd_, buffer, sizeof(buffer) / sizeof(char));
            if(std::string(buffer) == "CabinetWClass"){
              // nop
            }
            else{
              ::SetWindowLong(hwnd_, GWL_STYLE, style ^ WS_CAPTION ^ WS_THICKFRAME );
            }
          }
        }
      }
      void unmanage(HWND const hwnd_){
        auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
        if(it != m_managed_hwnds.end()){
          m_managed_hwnds.erase(it);
          RECT const rect = m_managed_hwnd_to_rect[hwnd_];
          LONG const width = rect.right - rect.left;
          LONG const height = rect.bottom - rect.top;
          ::SetWindowPos(hwnd_, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
          ::SetWindowLong(hwnd_, GWL_STYLE, m_managed_hwnd_to_style[hwnd_]);
        }
      }
      void print_managed_windows(){
        HWND const active_hwnd = ::GetActiveWindow();
        HWND const foreground_hwnd = ::GetForegroundWindow();
        char buffer[256];
        for(auto hwnd : m_managed_hwnds){
          ::GetWindowText(hwnd, buffer, sizeof(buffer) / sizeof(char));
          std::cout << "Hwnd:" << hwnd << std::endl;
          std::cout << "Title:" << buffer << std::endl;
          ::GetClassName(hwnd, buffer, sizeof(buffer) / sizeof(char));
          std::cout << "ClassName:" << buffer << std::endl;
          std::cout << "Active:" << (hwnd == active_hwnd) << std::endl;
          std::cout << "Foreground:" << (hwnd == foreground_hwnd) << std::endl;
          std::cout << "Style:" << m_managed_hwnd_to_style[hwnd] << std::endl;

          // HWND const parent = ::GetParent(hwnd_);
          // HWND const owner = ::GetWindow(hwnd_, GW_OWNER);
          // std::cout << "ParentHwnd:" << parent << std::endl;
          // std::cout << "OwnerHwnd:" << owner << std::endl;
          // std::cout << "IsWindow:" << ::IsWindow(hwnd_) << std::endl;
          // std::cout << "Visible:" << ::IsWindowVisible(hwnd_) << std::endl;
          std::cout << std::endl;
        }
      }
      UINT const start(){
        typedef BOOL (* RegisterShellHookWindowProc) (HWND);
        RegisterShellHookWindowProc RegisterShellHookWindow \
          = (RegisterShellHookWindowProc) ::GetProcAddress( ::GetModuleHandle("USER32.DLL"), "RegisterShellHookWindow");
        if (!RegisterShellHookWindow){
          Tile::die("Could not find RegisterShellHookWindow");
        }

        ::EnumWindows(scan, 0);

        RegisterShellHookWindow(m_main_hwnd);

        arrange();

        ::MSG msg;
        while (::GetMessage(&msg, NULL, 0, 0) > 0){
          ::TranslateMessage(&msg);
          ::DispatchMessage(&msg);
        }

        ::DeregisterShellHookWindow(m_main_hwnd);

        return msg.wParam;
      };
      void regist_key( UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_){
        std::shared_ptr<Key> p(new Key(m_main_hwnd, mod_, k_, f_));
        m_keys.push_back(p);
      };
      void call_key_method(UINT const& i_) const{
        for(auto key : m_keys){
          if(key->hash() == i_){
            key->call();
          }
        }
      };
      HWND const& get_main_hwnd() const{
        return m_main_hwnd;
      };
      long get_statusline_width() const{
        RECT rect = Tile::get_window_area();
        long const width = rect.right - rect.left;
        return width;
      };
      long get_statusline_height() const{
        return 40;
      };
      void next_focus() const{
        bool is_next_focus = false;
        HWND const foreground_hwnd = ::GetForegroundWindow();
        if(0 < m_managed_hwnds.size()){
          ::SetForegroundWindow(m_managed_hwnds.at(0));
          for(auto hwnd : m_managed_hwnds){
            if(foreground_hwnd == hwnd){
              is_next_focus = true;
            }
            else{
              if(is_next_focus){
                ::SetForegroundWindow(hwnd);
                break;
              }
            }
          }
        }
      }
      void prev_focus() const{
        bool is_next_focus = false;
        HWND const foreground_hwnd = ::GetForegroundWindow();
        if(0 < m_managed_hwnds.size()){
          ::SetForegroundWindow(m_managed_hwnds.at(m_managed_hwnds.size() - 1));
          for(auto it = m_managed_hwnds.rbegin(); it < m_managed_hwnds.rend(); it++){
            if(foreground_hwnd == *it){
              is_next_focus = true;
            }
            else{
              if(is_next_focus){
                ::SetForegroundWindow(*it);
                break;
              }
            }
          }
        }
      }
      void focus_window_to_first(){
        HWND const foreground_hwnd = ::GetForegroundWindow();
        if(0 < m_managed_hwnds.size()){
          for(auto it = std::begin(m_managed_hwnds); it < std::end(m_managed_hwnds); it++){
            if(foreground_hwnd == *it){
              m_managed_hwnds.erase(it);
              break;
            }
          }
          m_managed_hwnds.push_front(foreground_hwnd);
          arrange();
        }
      }
      void redraw_statusline(){
        ::PostMessage(m_statusline_hwnd, WM_PAINT, 0, 0);
      }
  };
}
std::shared_ptr<Tile::TilingWindowManager> g_p_tile_window_manager(nullptr);

void arrange1(std::deque<HWND> const& hwnds_){
  RECT rect = Tile::get_window_area();
  rect.top += g_p_tile_window_manager->get_statusline_height();
  long const width = rect.right - rect.left;
  long const height = rect.bottom - rect.top;

  switch(hwnds_.size()){
    case 1:
      ::SetWindowPos(hwnds_.at(0), HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
      break;
    case 2:
      ::SetWindowPos(hwnds_.at(0), HWND_TOP, rect.left            , rect.top, width / 2, height, SWP_NOACTIVATE);
      ::SetWindowPos(hwnds_.at(1), HWND_TOP, rect.left + width / 2, rect.top, width / 2, height, SWP_NOACTIVATE);
      break;
    case 3:
      ::SetWindowPos(hwnds_.at(0), HWND_TOP, rect.left            , rect.top             , width / 2, height    , SWP_NOACTIVATE);
      ::SetWindowPos(hwnds_.at(1), HWND_TOP, rect.left + width / 2, rect.top             , width / 2, height / 2, SWP_NOACTIVATE);
      ::SetWindowPos(hwnds_.at(2), HWND_TOP, rect.left + width / 2, rect.top + height / 2, width / 2, height / 2, SWP_NOACTIVATE);
      break;
    default:
      ::SetWindowPos(hwnds_.at(0), HWND_TOP, rect.left            , rect.top             , width / 2, height / 2, SWP_NOACTIVATE);
      ::SetWindowPos(hwnds_.at(1), HWND_TOP, rect.left + width / 2, rect.top             , width / 2, height / 2, SWP_NOACTIVATE);
      ::SetWindowPos(hwnds_.at(2), HWND_TOP, rect.left            , rect.top + height / 2, width / 2, height / 2, SWP_NOACTIVATE);
      ::SetWindowPos(hwnds_.at(3), HWND_TOP, rect.left + width / 2, rect.top + height / 2, width / 2, height / 2, SWP_NOACTIVATE);
      break;
  }
}

void keyevent_quit(){
  ::PostMessage(g_p_tile_window_manager->get_main_hwnd(), WM_CLOSE, 0, 0);
}
void keyevent_print_managed_windows(){
  g_p_tile_window_manager->print_managed_windows();
}
void keyevent_arrange(){
  g_p_tile_window_manager->arrange();
}
void keyevent_run_gvim(){
  ::ShellExecute(NULL, NULL, "C:\\vim\\src\\gvim.exe", "", NULL, SW_SHOWDEFAULT);
}
void keyevent_next_focus(){
  g_p_tile_window_manager->next_focus();
}
void keyevent_prev_focus(){
  g_p_tile_window_manager->prev_focus();
}
void keyevent_focus_window_to_first(){
  g_p_tile_window_manager->focus_window_to_first();
}

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

  g_p_tile_window_manager.reset(
      new Tile::TilingWindowManager(std::shared_ptr<Tile::Layout>(new Tile::Layout(arrange1)), hInstance_)
      );

  unsigned long const MODKEY = MOD_ALT | MOD_CONTROL;

  g_p_tile_window_manager->regist_key( MODKEY,  'Q', keyevent_quit );
  g_p_tile_window_manager->regist_key( MODKEY,  'P', keyevent_print_managed_windows );
  g_p_tile_window_manager->regist_key( MODKEY,  'A', keyevent_arrange );
  g_p_tile_window_manager->regist_key( MODKEY,  'V', keyevent_run_gvim );
  g_p_tile_window_manager->regist_key( MODKEY,  'J', keyevent_next_focus );
  g_p_tile_window_manager->regist_key( MODKEY,  'K', keyevent_prev_focus );
  g_p_tile_window_manager->regist_key( MODKEY,  'T', keyevent_focus_window_to_first );

  g_p_tile_window_manager->start();


#ifdef DEBUG
  if(use_console){
    ::FreeConsole();
  }
  else{
  }
#endif

  return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_){
  if(g_p_tile_window_manager != nullptr){
    g_p_tile_window_manager->redraw_statusline();
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
      return 0;
    case WM_CREATE:
    case WM_TIMER:
    case WM_PAINT:
      PAINTSTRUCT ps;
      ::BeginPaint(hwnd_, &ps);
      HDC hdc = ::GetWindowDC(hwnd_);
      ::SelectObject(hdc, ::GetStockObject(BLACK_BRUSH));
      ::Rectangle(hdc , 0, 0, g_p_tile_window_manager->get_statusline_width(), g_p_tile_window_manager->get_statusline_height());
      RECT rect;
      rect.left = 10;
      rect.top = 10;
      rect.right = g_p_tile_window_manager->get_statusline_width();
      rect.bottom = g_p_tile_window_manager->get_statusline_height();
      char buffer[256];
      ::SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
      ::SetBkColor(hdc, RGB(0x00, 0x00, 0x00));
      ::GetWindowText(::GetForegroundWindow(), buffer, sizeof(buffer) / sizeof(char));
      ::DrawText(hdc, buffer, -1, &rect, DT_LEFT | DT_WORDBREAK);
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


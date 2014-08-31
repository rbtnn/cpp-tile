
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

// RegisterHotKey function
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646309(v=vs.85).aspx
// RegisterHotKey
// http://msdn.microsoft.com/ja-jp/library/cc411006.aspx

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
      std::string m_layout_name;
      std::function<void(std::deque<HWND> const& hwnds_)> m_f;

    public:
      Layout(std::string layout_name_, std::function<void(std::deque<HWND> const& hwnds_)> f_)
        : m_layout_name(layout_name_), m_f(f_){
        };

      std::string get_layout_name() const{
        return m_layout_name;
      }
      void arrange(std::deque<HWND> const& hwnds_){
        m_f(hwnds_);
      }
  };
  class TilingWindowManager{
    private:
      HINSTANCE m_hInstance;

      HWND m_main_hwnd;
      std::string m_main_class_name;
      HWND m_statusline_hwnd;
      std::string m_statusline_class_name;

      UINT m_shellhookid;

      std::vector<Tile::Layout> m_layouts;
      std::vector<Tile::Layout>::iterator m_layout_it;

      std::vector<std::shared_ptr<Key> > m_keys;
      std::deque<HWND> m_managed_hwnds;
      std::map<HWND, RECT> m_managed_hwnd_to_rect;
      std::map<HWND, LONG> m_managed_hwnd_to_style;

      std::vector<std::string> split(const std::string& input_, char delimiter_){
        std::istringstream stream(input_);
        std::string field;
        std::vector<std::string> result;
        while (std::getline(stream, field, delimiter_)) {
          result.push_back(field);
        }
        return result;
      }
      std::vector<std::string> get_config_settings_ignore_classnames(){
        char buffer[1024];
        ::GetPrivateProfileString("settings", "IGNORE_CLASSNAMES", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
        return split(std::string(buffer), ',');
      }
      std::vector<std::string> get_config_settings_not_apply_style_to_classnames(){
        char buffer[1024];
        ::GetPrivateProfileString("settings", "NOT_APPLY_STYLE_TO_CLASSNAMES", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
        return split(std::string(buffer), ',');
      }
      std::map<std::string, std::string> get_config_keys(){
        std::map<std::string, std::string> m;
        char buffer[1024];
        ::GetPrivateProfileSection("keys", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
        std::string line = "";
        for(unsigned int i=0; i < (sizeof(buffer) / sizeof(char)) - 1; i++){
          if(buffer[i] == '\0'){
            auto xs = split(line, '=');
            line = "";
            if(1 < xs.size()){
              m.insert(std::map<std::string, std::string>::value_type(xs.at(0), xs.at(1)));
            }
            if(buffer[i + 1] == '\0'){
              break;
            }
          }
          else{
            line += buffer[i];
          }
        }
        return m;
      }

      void init_main(){
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

        for(auto classname : get_config_settings_ignore_classnames()){
          if(std::string(buffer) == classname){ return false; }
        }
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
      void set_style(HWND hwnd_){
        if(is_manageable(hwnd_)){
          char buffer[256];
          LONG const style = ::GetWindowLong(hwnd_, GWL_STYLE);
          ::GetClassName(hwnd_, buffer, sizeof(buffer) / sizeof(char));
          bool b = true;
          for(auto classname : get_config_settings_not_apply_style_to_classnames()){
            if(std::string(buffer) == classname){
              b = false;
            }
          }
          if(b){
            ::SetWindowLong(hwnd_, GWL_STYLE, style ^ WS_CAPTION ^ WS_THICKFRAME );
          }
        }
      }
      void regist_key(std::string key, void (Tile::TilingWindowManager::* f_)()){
        unsigned long const MODKEY = MOD_ALT | MOD_CONTROL;
        std::map<std::string, std::string> m = get_config_keys();
        auto it = m.find(key);
        if(it != std::end(m)){
          unsigned int const n = std::atoi(m[key].c_str());
          m_keys.push_back(std::shared_ptr<Key>(new Key(m_main_hwnd, MODKEY, n, std::bind(f_, this))));
        }
      }

    public:
      TilingWindowManager(HINSTANCE const& hInstance_, std::string main_classname_, std::vector<Tile::Layout> layouts_){
        m_hInstance = hInstance_;
        m_layouts = layouts_;
        m_layout_it = std::begin(m_layouts);

        m_main_class_name = main_classname_;
        init_main();
        m_statusline_class_name = "TileStatusLine";
        init_statusline();

        m_shellhookid = ::RegisterWindowMessage("SHELLHOOK");
      }
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

      std::string get_inifile_path(){
        char szFileName[1024];
        ::GetModuleFileName( NULL, szFileName, MAX_PATH);
        std::string str = std::string(szFileName);
        std::string::size_type pos = str.find(".exe", 0);
        if(pos != std::string::npos){
          str.replace(pos, 4, ".ini");
        }
        else{
          str = "";
        }
        return str;
      }

      void exit_tile(){
        ::PostMessage(m_main_hwnd, WM_CLOSE, 0, 0);
      }
      void kill_client(){
        HWND const foreground_hwnd = ::GetForegroundWindow();
        ::PostMessage(foreground_hwnd, WM_CLOSE, 0, 0);
      }
      void arrange(){
        if(m_layout_it != std::end(m_layouts)){
          m_layout_it->arrange(m_managed_hwnds);
        }
      }
      void next_layout(){
        m_layout_it++;
        if(m_layout_it == std::end(m_layouts)){
          m_layout_it = std::begin(m_layouts);
        }
        arrange();
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
      void next_focus(){
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
      void prev_focus(){
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
      void focus_window_to_master(){
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

      void manage(HWND hwnd_){
        if(is_manageable(hwnd_)){
          auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
          if(it == m_managed_hwnds.end()){
            RECT rect;
            ::GetWindowRect(hwnd_, &rect);
            m_managed_hwnds.push_back(hwnd_);
            m_managed_hwnd_to_rect.insert(std::map<HWND, RECT>::value_type(hwnd_, rect));
            m_managed_hwnd_to_style.insert(std::map<HWND, LONG>::value_type(hwnd_, ::GetWindowLong(hwnd_, GWL_STYLE)));
            set_style(hwnd_);
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
      UINT const start(){
        typedef BOOL (* RegisterShellHookWindowProc) (HWND);
        RegisterShellHookWindowProc RegisterShellHookWindow \
          = (RegisterShellHookWindowProc) ::GetProcAddress( ::GetModuleHandle("USER32.DLL"), "RegisterShellHookWindow");
        if (!RegisterShellHookWindow){
          Tile::die("Could not find RegisterShellHookWindow");
        }

        ::EnumWindows(scan, 0);

        RegisterShellHookWindow(m_main_hwnd);

        regist_key("focus_window_to_master", &TilingWindowManager::focus_window_to_master);
        regist_key("next_focus", &TilingWindowManager::next_focus);
        regist_key("next_layout", &TilingWindowManager::next_layout);
        regist_key("prev_focus", &TilingWindowManager::prev_focus);
        regist_key("print_managed_windows", &TilingWindowManager::print_managed_windows);
        regist_key("exit_tile", &TilingWindowManager::exit_tile);
        regist_key("kill_client", &TilingWindowManager::kill_client);

        arrange();

        ::MSG msg;
        while (::GetMessage(&msg, NULL, 0, 0) > 0){
          ::TranslateMessage(&msg);
          ::DispatchMessage(&msg);
        }

        ::DeregisterShellHookWindow(m_main_hwnd);

        return msg.wParam;
      }
      void call_key_method(UINT const& i_) const{
        for(auto key : m_keys){
          if(key->hash() == i_){
            key->call();
          }
        }
      }
      long get_statusline_width() const{
        RECT rect = Tile::get_window_area();
        long const width = rect.right - rect.left;
        return width;
      }
      long get_statusline_height() const{
        return 40;
      }
      std::string get_layout_name(){
        if(0 < m_layouts.size()){
          return m_layout_it->get_layout_name();
        }
        else{
          return "";
        }
      }
      void redraw_statusline(){
        ::PostMessage(m_statusline_hwnd, WM_PAINT, 0, 0);
      }
  };
}
std::shared_ptr<Tile::TilingWindowManager> g_p_tile_window_manager(nullptr);

void arrange(std::deque<HWND> const& hwnds_){
  RECT rect = Tile::get_window_area();
  rect.top += g_p_tile_window_manager->get_statusline_height();
  long const width = rect.right - rect.left;
  long const height = rect.bottom - rect.top;

  switch(hwnds_.size()){
    case 1:
      if(0 < hwnds_.size()){
        ::ShowWindow(hwnds_.at(0), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(0), HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
      }
      break;
    case 2:
      if(1 < hwnds_.size()){
        ::ShowWindow(hwnds_.at(0), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(0), HWND_TOP, rect.left            , rect.top, width / 2, height, SWP_NOACTIVATE);
        ::ShowWindow(hwnds_.at(1), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(1), HWND_TOP, rect.left + width / 2, rect.top, width / 2, height, SWP_NOACTIVATE);
      }
      break;
    case 3:
      if(2 < hwnds_.size()){
        ::ShowWindow(hwnds_.at(0), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(0), HWND_TOP, rect.left            , rect.top             , width / 2, height    , SWP_NOACTIVATE);
        ::ShowWindow(hwnds_.at(1), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(1), HWND_TOP, rect.left + width / 2, rect.top             , width / 2, height / 2, SWP_NOACTIVATE);
        ::ShowWindow(hwnds_.at(2), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(2), HWND_TOP, rect.left + width / 2, rect.top + height / 2, width / 2, height / 2, SWP_NOACTIVATE);
      }
      break;
    default:
      if(3 < hwnds_.size()){
        ::ShowWindow(hwnds_.at(0), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(0), HWND_TOP, rect.left            , rect.top             , width / 2, height / 2, SWP_NOACTIVATE);
        ::ShowWindow(hwnds_.at(1), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(1), HWND_TOP, rect.left + width / 2, rect.top             , width / 2, height / 2, SWP_NOACTIVATE);
        ::ShowWindow(hwnds_.at(2), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(2), HWND_TOP, rect.left            , rect.top + height / 2, width / 2, height / 2, SWP_NOACTIVATE);
        ::ShowWindow(hwnds_.at(3), SW_SHOWNORMAL);
        ::SetWindowPos(hwnds_.at(3), HWND_TOP, rect.left + width / 2, rect.top + height / 2, width / 2, height / 2, SWP_NOACTIVATE);
      }
      break;
  }
}
void arrange_maximal(std::deque<HWND> const& hwnds_){
  RECT rect = Tile::get_window_area();
  rect.top += g_p_tile_window_manager->get_statusline_height();
  long const width = rect.right - rect.left;
  long const height = rect.bottom - rect.top;
  HWND const foreground_hwnd = ::GetForegroundWindow();
  for(auto hwnd : hwnds_){
    ::SetWindowPos((hwnd == foreground_hwnd) ? foreground_hwnd : hwnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
  }
}

void keyevent_run_gvim(){
  ::ShellExecute(NULL, NULL, "C:\\vim\\src\\gvim.exe", "", NULL, SW_SHOWDEFAULT);
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
    g_p_tile_window_manager.reset(new Tile::TilingWindowManager(hInstance_, "Tile", {
          Tile::Layout("arrange", arrange),
          Tile::Layout("arrange_maximal", arrange_maximal),
          }));
    std::string const path = g_p_tile_window_manager->get_inifile_path();
    if(exist_file(path)){
      g_p_tile_window_manager->start();
    }
    else{
      ::MessageBox(NULL, ("'" + path + "' does not exist.").c_str(), "Error", MB_ICONERROR);
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
  if(g_p_tile_window_manager != nullptr){
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
        ::SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
        ::SetBkColor(hdc, RGB(0x00, 0x00, 0x00));

        char buffer[1024];
        ::GetWindowText(::GetForegroundWindow(), buffer, sizeof(buffer) / sizeof(char));
        std::string const windowtext = std::string(buffer);
        ::GetClassName(::GetForegroundWindow(), buffer, sizeof(buffer) / sizeof(char));
        std::string const classname = std::string(buffer);

        ::DrawText(hdc, ("<" + g_p_tile_window_manager->get_layout_name() + ">[" + classname + "] " + windowtext).c_str(),
            -1, &rect, DT_LEFT | DT_WORDBREAK);
        ::ReleaseDC(hwnd_, hdc);
        ::EndPaint(hwnd_, &ps);
        break;
    }
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


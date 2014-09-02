
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
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK StatusLineWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK BorderWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK scan(HWND, LPARAM);

// RegisterHotKey function
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646309(v=vs.85).aspx
// RegisterHotKey
// http://msdn.microsoft.com/ja-jp/library/cc411006.aspx

namespace Tile{
  void die(std::string const& msg);
  RECT get_window_area();

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
  class Workspace{
    private:
      std::string m_workspace_name;

      std::deque<HWND> m_managed_hwnds;
      std::map<HWND, RECT> m_managed_hwnd_to_rect;
      std::map<HWND, LONG> m_managed_hwnd_to_style;

      void set_style(HWND hwnd_, std::vector<std::string> classnames_){
        char buffer[256];
        LONG const style = ::GetWindowLong(hwnd_, GWL_STYLE);
        ::GetClassName(hwnd_, buffer, sizeof(buffer) / sizeof(char));
        bool b = true;
        for(auto classname : classnames_){
          if(std::string(buffer) == classname){
            b = false;
          }
        }
        if(b){
          ::SetWindowLong(hwnd_, GWL_STYLE, style ^ WS_CAPTION ^ WS_THICKFRAME );
        }
      }

    public:
      Workspace(std::string workspace_name_): m_workspace_name(workspace_name_){
      }

      unsigned int size(){
        return m_managed_hwnds.size();
      }
      HWND at(unsigned int i){
        return m_managed_hwnds.at(i);
      }

      std::deque<HWND>::iterator begin(){
        return std::begin(m_managed_hwnds);
      }
      std::deque<HWND>::iterator end(){
        return std::begin(m_managed_hwnds);
      }
      std::deque<HWND>::reverse_iterator rbegin(){
        return m_managed_hwnds.rbegin();
      }
      std::deque<HWND>::reverse_iterator rend(){
        return m_managed_hwnds.rend();
      }

      bool is_managed(HWND hwnd_){
        auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
        return (it != std::end(m_managed_hwnds));
      }
      void manage(HWND hwnd_, std::vector<std::string> classnames_){
        if(!is_managed(hwnd_)){
          RECT rect;
          ::GetWindowRect(hwnd_, &rect);
          m_managed_hwnds.push_front(hwnd_);
          m_managed_hwnd_to_rect.insert(std::map<HWND, RECT>::value_type(hwnd_, rect));
          m_managed_hwnd_to_style.insert(std::map<HWND, LONG>::value_type(hwnd_, ::GetWindowLong(hwnd_, GWL_STYLE)));
          set_style(hwnd_, classnames_);
        }
      }
      void unmanage(HWND const hwnd_){
        auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
        if(it != std::end(m_managed_hwnds)){
          m_managed_hwnds.erase(it);
          RECT const rect = m_managed_hwnd_to_rect[hwnd_];
          LONG const width = rect.right - rect.left;
          LONG const height = rect.bottom - rect.top;
          ::SetWindowPos(hwnd_, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
          ::SetWindowLong(hwnd_, GWL_STYLE, m_managed_hwnd_to_style[hwnd_]);
        }
      }
      void hide(HWND const hwnd_){
        auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
        if(it != std::end(m_managed_hwnds)){
          ::ShowWindow(hwnd_, SW_HIDE);
        }
      }

      std::deque<HWND> const& get_managed_hwnds(){
        return m_managed_hwnds;
      }

      std::string get_workspace_name() const{
        return m_workspace_name;
      }
  };
  class TilingWindowManager{
    private:
      HINSTANCE m_hInstance;
      UINT m_shellhookid;
      HWND m_main_hwnd;
      std::string m_main_class_name;
      HWND m_statusline_hwnd;
      std::string m_statusline_class_name;
      HWND m_border_left_hwnd;
      HWND m_border_right_hwnd;
      HWND m_border_top_hwnd;
      HWND m_border_bottom_hwnd;
      std::string m_border_class_name;

      std::vector<std::shared_ptr<Key> > m_keys;

      std::vector<Tile::Workspace> m_workspaces;
      std::vector<Tile::Workspace>::iterator m_workspace_it;

      std::vector<Tile::Layout> m_layouts;
      std::vector<Tile::Layout>::iterator m_layout_it;

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
        winClass.lpfnWndProc = MainWndProc;
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
          Tile::die("Error registering window class(main)");
        }

        m_main_hwnd = ::CreateWindowEx(0,
            m_main_class_name.c_str(), m_main_class_name.c_str(),
            0, 0, 0, 0, 0, NULL, NULL,
            m_hInstance, NULL);
        if (!m_main_hwnd){
          Tile::die("Error creating window(main)");
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
          Tile::die("Error creating window(statusline)");
        }
      }
      void init_border(){
        WNDCLASSEX winClass;
        winClass.cbSize = sizeof(::WNDCLASSEX);
        winClass.style = 0;
        winClass.lpfnWndProc = BorderWndProc;
        winClass.cbClsExtra = 0;
        winClass.cbWndExtra = 0;
        winClass.hInstance = m_hInstance;
        winClass.hIcon = NULL;
        winClass.hIconSm = NULL;
        winClass.hCursor = NULL;
        winClass.hbrBackground = NULL;
        winClass.lpszMenuName = NULL;
        winClass.lpszClassName = m_border_class_name.c_str();
        ATOM atom = ::RegisterClassEx(&winClass);
        if (!atom){
          Tile::die("Error registering window class(border)");
        }

        m_border_left_hwnd = ::CreateWindowEx(WS_EX_TOOLWINDOW,
            m_border_class_name.c_str(), m_border_class_name.c_str(),
            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
            0, 0, 0, 0, NULL, NULL,
            m_hInstance, NULL);
        if (!m_border_left_hwnd){
          Tile::die("Error creating window(border)");
        }
        m_border_right_hwnd = ::CreateWindowEx(WS_EX_TOOLWINDOW,
            m_border_class_name.c_str(), m_border_class_name.c_str(),
            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
            0, 0, 0, 0, NULL, NULL,
            m_hInstance, NULL);
        if (!m_border_right_hwnd){
          Tile::die("Error creating window(border)");
        }
        m_border_top_hwnd = ::CreateWindowEx(WS_EX_TOOLWINDOW,
            m_border_class_name.c_str(), m_border_class_name.c_str(),
            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
            0, 0, 0, 0, NULL, NULL,
            m_hInstance, NULL);
        if (!m_border_top_hwnd){
          Tile::die("Error creating window(border)");
        }
        m_border_bottom_hwnd = ::CreateWindowEx(WS_EX_TOOLWINDOW,
            m_border_class_name.c_str(), m_border_class_name.c_str(),
            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
            0, 0, 0, 0, NULL, NULL,
            m_hInstance, NULL);
        if (!m_border_bottom_hwnd){
          Tile::die("Error creating window(border)");
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

        m_workspaces = {
          Tile::Workspace("1"),
          Tile::Workspace("2"),
        };
        m_workspace_it = std::begin(m_workspaces);

        m_layouts = layouts_;
        m_layout_it = std::begin(m_layouts);

        m_main_class_name = main_classname_;
        init_main();
        m_statusline_class_name = "TileStatusLine";
        init_statusline();
        m_border_class_name = "TileBorder";
        init_border();

        m_shellhookid = ::RegisterWindowMessage("SHELLHOOK");
      }
      ~TilingWindowManager(){
        for(auto it = std::begin(m_workspaces); it < std::end(m_workspaces); it++){
          for(auto hwnd : it->get_managed_hwnds()){
            it->unmanage(hwnd);
          }
        }
        if (!m_main_hwnd){
          ::DestroyWindow(m_main_hwnd);
        }
        if (!m_statusline_hwnd){
          ::DestroyWindow(m_statusline_hwnd);
        }
        if (!m_border_left_hwnd){
          ::DestroyWindow(m_border_left_hwnd);
        }
        if (!m_border_right_hwnd){
          ::DestroyWindow(m_border_right_hwnd);
        }
        if (!m_border_top_hwnd){
          ::DestroyWindow(m_border_top_hwnd);
        }
        if (!m_border_bottom_hwnd){
          ::DestroyWindow(m_border_bottom_hwnd);
        }
        if (!m_hInstance){
          ::UnregisterClass(m_statusline_class_name.c_str(), m_hInstance);
          ::UnregisterClass(m_main_class_name.c_str(), m_hInstance);
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
        regist_key("exit_tile", &TilingWindowManager::exit_tile);
        regist_key("kill_client", &TilingWindowManager::kill_client);
        regist_key("run_shell", &TilingWindowManager::run_shell);
        regist_key("workspace_1", &TilingWindowManager::workspace_1);
        regist_key("workspace_2", &TilingWindowManager::workspace_2);

        arrange();

        ::MSG msg;
        while (::GetMessage(&msg, NULL, 0, 0) > 0){
          ::TranslateMessage(&msg);
          ::DispatchMessage(&msg);
        }

        ::DeregisterShellHookWindow(m_main_hwnd);

        return msg.wParam;
      }

      void manage(HWND hwnd_){
        if(is_manageable(hwnd_)){
          m_workspace_it->manage(hwnd_, get_config_settings_not_apply_style_to_classnames());
        }
      }
      void unmanage(HWND hwnd_){
        m_workspace_it->unmanage(hwnd_);
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

      void run_shell(){
        ::ShellExecute(NULL, NULL, "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe", "", NULL, SW_SHOWDEFAULT);
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
          m_layout_it->arrange(m_workspace_it->get_managed_hwnds());
        }
        if(is_manageable(::GetForegroundWindow())){
          unsigned int n = 5;
          RECT rect;
          ::GetWindowRect(::GetForegroundWindow(), &rect);
          ::SetWindowPos(m_border_left_hwnd, HWND_TOPMOST,
              rect.left, rect.top, n, rect.bottom - rect.top, SWP_NOACTIVATE);
          ::SetWindowPos(m_border_right_hwnd, HWND_TOPMOST,
              rect.right - n, rect.top, n, rect.bottom - rect.top, SWP_NOACTIVATE);
          ::SetWindowPos(m_border_top_hwnd, HWND_TOPMOST,
              rect.left, rect.top, rect.right - rect.left, n, SWP_NOACTIVATE);
          ::SetWindowPos(m_border_bottom_hwnd, HWND_TOPMOST,
              rect.left, rect.bottom - n, rect.right - rect.left, n, SWP_NOACTIVATE);

          ::ShowWindow(m_border_top_hwnd, SW_SHOWNORMAL);
          ::ShowWindow(m_border_bottom_hwnd, SW_SHOWNORMAL);
          ::ShowWindow(m_border_left_hwnd, SW_SHOWNORMAL);
          ::ShowWindow(m_border_right_hwnd, SW_SHOWNORMAL);
        }
        else{
          ::ShowWindow(m_border_top_hwnd, SW_HIDE);
          ::ShowWindow(m_border_bottom_hwnd, SW_HIDE);
          ::ShowWindow(m_border_left_hwnd, SW_HIDE);
          ::ShowWindow(m_border_right_hwnd, SW_HIDE);
        }
      }
      void next_layout(){
        m_layout_it++;
        if(m_layout_it == std::end(m_layouts)){
          m_layout_it = std::begin(m_layouts);
        }
        arrange();
      }
      void next_focus(){
        bool is_next_focus = false;
        HWND const foreground_hwnd = ::GetForegroundWindow();
        if(0 < m_workspace_it->size()){
          ::SetForegroundWindow(m_workspace_it->at(0));
          for(auto hwnd : m_workspace_it->get_managed_hwnds()){
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
        if(0 < m_workspace_it->size()){
          ::SetForegroundWindow(m_workspace_it->at(m_workspace_it->size() - 1));
          for(auto it = m_workspace_it->rbegin(); it < m_workspace_it->rend(); it++){
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
      void workspace_1(){
        workspace_of(0);
      }
      void workspace_2(){
        workspace_of(1);
      }
      void focus_window_to_master(){
        HWND const foreground_hwnd = ::GetForegroundWindow();
        if(0 < m_workspace_it->size()){
          m_workspace_it->unmanage(foreground_hwnd);
          m_workspace_it->manage(foreground_hwnd, get_config_settings_not_apply_style_to_classnames());
          arrange();
        }
      }

      // void print_managed_windows(){
      //   HWND const active_hwnd = ::GetActiveWindow();
      //   HWND const foreground_hwnd = ::GetForegroundWindow();
      //   char buffer[256];
      //   for(unsigned int i = 0; i < m_workspace_it->size(); i++){
      //     auto hwnd = m_workspace_it->at(i);
      //     ::GetWindowText(hwnd, buffer, sizeof(buffer) / sizeof(char));
      //     std::cout << "No:" << i << std::endl;
      //     std::cout << "Hwnd:" << hwnd << std::endl;
      //     std::cout << "Title:" << buffer << std::endl;
      //     ::GetClassName(hwnd, buffer, sizeof(buffer) / sizeof(char));
      //     std::cout << "ClassName:" << buffer << std::endl;
      //     std::cout << "Active:" << (hwnd == active_hwnd) << std::endl;
      //     std::cout << "Foreground:" << (hwnd == foreground_hwnd) << std::endl;
      //     std::cout << "Style:" << m_managed_hwnd_to_style[hwnd] << std::endl;
      //     std::cout << std::endl;
      //   }
      //   std::cout << std::endl;
      // }

      void workspace_of(unsigned int const i){
        unsigned int n = 0;
        if(i < m_workspaces.size()){
          for(auto it = std::begin(m_workspaces); it < std::end(m_workspaces); it++){
            if(n == i){
              if(m_workspace_it != it){
                for(auto hwnd : m_workspace_it->get_managed_hwnds()){
                  m_workspace_it->hide(hwnd);
                }
                m_workspace_it = it;
              }
              arrange();
              break;
            }
            n++;
          }
        }
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
      static long get_statusline_height(){
        return 40;
      }
      void redraw_statusline(){
        ::PostMessage(m_statusline_hwnd, WM_PAINT, 0, 0);
      }
      std::string get_layout_name(){
        if(0 < m_layouts.size()){
          return m_layout_it->get_layout_name();
        }
        else{
          return "";
        }
      }
      std::string get_workspace_name() const{
        return m_workspace_it->get_workspace_name();
      }
  };

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
    wa.top += TilingWindowManager::get_statusline_height();
    return wa;
  }
}
std::shared_ptr<Tile::TilingWindowManager> g_p_tile_window_manager(nullptr);

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

void arrange(std::deque<HWND> const& hwnds_){
  RECT window_area = Tile::get_window_area();
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
void arrange_maximal(std::deque<HWND> const& hwnds_){
  RECT window_area = Tile::get_window_area();
  long const width = window_area.right - window_area.left;
  long const height = window_area.bottom - window_area.top;
  HWND const foreground_hwnd = ::GetForegroundWindow();
  for(auto hwnd : hwnds_){
    resize_window(window_area, ((hwnd == foreground_hwnd) ? foreground_hwnd : hwnd), 0, 0, width, height);
  }
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
      ::SetTimer(hwnd_, 1, 1000, NULL);
      break;

    case WM_TIMER:
    case WM_PAINT:
      if(g_p_tile_window_manager != nullptr){
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

        SYSTEMTIME stTime;
        ::GetLocalTime(&stTime);

        std::stringstream ss;
        ss << "[" << stTime.wYear << "/" << stTime.wMonth << "/" << stTime.wDay
          << " " << stTime.wHour << ":" << stTime.wMinute << ":" << stTime.wSecond << "]"
          << "[" << g_p_tile_window_manager->get_workspace_name() << "]"
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


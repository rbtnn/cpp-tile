
#include <tile/common_headers.h>
#include <tile/common_functions.h>
#include <tile/wndproc_functions.h>
#include <tile/TilingWindowManager.h>
#include <tile/ConfigReader.h>
#include <tile/Workspace.h>
#include <tile/HotKey.h>

namespace Tile{
  void TilingWindowManager::init_main(){
    WNDCLASSEX winClass = make_wndclassex(m_hInstance, MainWndProc, m_main_class_name);
    ATOM atom = ::RegisterClassEx(&winClass);
    if (!atom){
      die("Error registering window class(main)");
    }

    m_main_hwnd = ::CreateWindowEx(0,
        m_main_class_name.c_str(), m_main_class_name.c_str(),
        0, 0, 0, 0, 0, NULL, NULL,
        m_hInstance, NULL);
    if (!m_main_hwnd){
      die("Error creating window(main)");
    }
  }
  void TilingWindowManager::init_toast(){
    WNDCLASSEX winClass = make_wndclassex(m_hInstance, ToastWndProc, "TileToast");
    ATOM atom = ::RegisterClassEx(&winClass);
    if (!atom){
      die("Error registering window class(toast)");
    }
    m_toast_hwnd = make_toolwindow(m_hInstance, "TileToast");
    LONG const exstyle = ::GetWindowLong(m_toast_hwnd, GWL_EXSTYLE);
    ::SetWindowLong(m_toast_hwnd, GWL_EXSTYLE, exstyle | WS_EX_LAYERED);
    ::SetLayeredWindowAttributes(m_toast_hwnd, 0, 200, LWA_ALPHA);
    ::ShowWindow(m_toast_hwnd, SW_HIDE);
  }
  void TilingWindowManager::init_border(){
    WNDCLASSEX winClass = make_wndclassex(m_hInstance, BorderWndProc, m_border_class_name);
    ATOM atom = ::RegisterClassEx(&winClass);
    if (!atom){
      die("Error registering window class(border)");
    }

    m_border_left_hwnd = make_toolwindow(m_hInstance, m_border_class_name);
    m_border_right_hwnd = make_toolwindow(m_hInstance, m_border_class_name);
    m_border_top_hwnd = make_toolwindow(m_hInstance, m_border_class_name);
    m_border_bottom_hwnd = make_toolwindow(m_hInstance, m_border_class_name);

    // first hide border
    toggle_border();
  }
  void TilingWindowManager::regist_key(std::string const& key, void (Tile::TilingWindowManager::* f_)()){
    std::map<std::string, Tile::HotKey> m = m_config->get_keys();
    auto it = m.find(key);
    if(it != std::end(m)){
      unsigned int const vk = it->second.get_vk();
      unsigned int const mod = it->second.get_fsModifiers();
      m_keys.push_back(std::shared_ptr<Key>(
            new Key(m_main_hwnd, key, mod, vk, std::bind(f_, this))
            ));
    }
  }
  void TilingWindowManager::unmanage_all(){
    for(auto it = std::begin(m_workspaces); it < std::end(m_workspaces); it++){
      auto const hwnds = it->get_managed_hwnds();
      for(auto hwnd : hwnds){
        it->unmanage(hwnd);
        recovery.load(hwnd);
      }
    }
  }

  void TilingWindowManager::rescan(){
    unmanage_all();
    ::EnumWindows(scan, 0);
    arrange();
    try_focus_managed_window();
  }
  void TilingWindowManager::show_information(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    int const exstyle = ::GetWindowLong(foreground_hwnd, GWL_EXSTYLE);
    HWND const parent = ::GetParent(foreground_hwnd);
    boost::optional<std::string> const s = m_workspace_it->get_layout_name();

    std::stringstream ss;

    ss << "-------------------------------" << "\n";
    ss << "WorkspaceName:" << m_workspace_it->get_workspace_name() << "\n";
    ss << "LayoutName:" << (s ? *s : "NULL") << "\n";
    ss << "Count:" << m_workspace_it->count() << "\n";
    ss << "-------------------------------" << "\n";
    ss << "ClassName:" << get_classname(foreground_hwnd) << "\n";
    ss << "hwnd(self):" << foreground_hwnd << "\n";
    ss << "is_manageable(self):" << is_manageable(foreground_hwnd) << "\n";
    ss << "hwnd(parent):" << parent << "\n";
    ss << "is_manageable(parent):" << is_manageable(parent) << "\n";
    ss << "WS_EX_TOOLWINDOW:" << (exstyle & WS_EX_TOOLWINDOW) << "\n";
    ss << "WS_EX_APPWINDOW:" << (exstyle & WS_EX_APPWINDOW) << "\n";
    ss << "-------------------------------" << "\n";

    set_toast_text(ss.str().c_str(), 360, 240);
  }
  void TilingWindowManager::toggle_border(){
    int const showCmd = (::IsWindowVisible(m_border_left_hwnd) == true) ? SW_HIDE : SW_SHOWNORMAL;
    for(auto hwnd : {
        m_border_top_hwnd,
        m_border_bottom_hwnd,
        m_border_left_hwnd,
        m_border_right_hwnd,
        }){
      ::ShowWindow(hwnd, showCmd);
    }
  }
  void TilingWindowManager::toggle_transparency_window(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    std::deque<HWND> const hwnds = m_workspace_it->get_managed_hwnds();
    auto const it = std::find(std::begin(hwnds), std::end(hwnds), foreground_hwnd);
    if(it != std::end(hwnds)){
      LONG const exstyle = ::GetWindowLong(foreground_hwnd, GWL_EXSTYLE);
      if(exstyle & WS_EX_LAYERED){
        ::SetWindowLong(foreground_hwnd, GWL_EXSTYLE, exstyle ^ WS_EX_LAYERED);
        ::SetLayeredWindowAttributes(foreground_hwnd, 0, 255, LWA_ALPHA);
      }
      else{
        ::SetWindowLong(foreground_hwnd, GWL_EXSTYLE, exstyle | WS_EX_LAYERED);
        ::SetLayeredWindowAttributes(foreground_hwnd, 0, 200, LWA_ALPHA);
      }
    }
  }
  void TilingWindowManager::swap_next(){
    auto const hwnds = m_workspace_it->get_managed_hwnds();
    HWND const foreground_hwnd = ::GetForegroundWindow();
    if(std::find(std::begin(hwnds), std::end(hwnds), foreground_hwnd) != std::end(hwnds)){
      for(auto it = std::begin(hwnds); it < std::end(hwnds);it++){
        if(*it == foreground_hwnd){
          it++;
          if(it == std::end(hwnds)){
            auto const top = hwnds.at(0);
            m_workspace_it->unmanage(foreground_hwnd);
            m_workspace_it->manage_front(foreground_hwnd, m_config->get_not_apply_style_to_classnames());
            m_workspace_it->unmanage(top);
            m_workspace_it->manage_back(top, m_config->get_not_apply_style_to_classnames());
          }
          else{
            m_workspace_it->unmanage(*it);
            m_workspace_it->manage_back(*it, m_config->get_not_apply_style_to_classnames());
            m_workspace_it->unmanage(foreground_hwnd);
            m_workspace_it->manage_back(foreground_hwnd, m_config->get_not_apply_style_to_classnames());
          }
        }
        else{
          m_workspace_it->unmanage(*it);
          m_workspace_it->manage_back(*it, m_config->get_not_apply_style_to_classnames());
        }
      }
    }
  }
  void TilingWindowManager::swap_previous(){
    auto const hwnds = m_workspace_it->get_managed_hwnds();
    HWND const foreground_hwnd = ::GetForegroundWindow();
    if(std::find(std::begin(hwnds), std::end(hwnds), foreground_hwnd) != std::end(hwnds)){
      for(auto it = std::begin(hwnds); it < std::end(hwnds);it++){
        if(*it == foreground_hwnd){
          if(it == std::begin(hwnds)){
            auto const tail = hwnds.at(hwnds.size() - 1);
            m_workspace_it->unmanage(foreground_hwnd);
            m_workspace_it->manage_back(foreground_hwnd, m_config->get_not_apply_style_to_classnames());
            m_workspace_it->unmanage(tail);
            m_workspace_it->manage_front(tail, m_config->get_not_apply_style_to_classnames());
            break;
          }
          else{
            it--;
            m_workspace_it->unmanage(foreground_hwnd);
            m_workspace_it->manage_back(foreground_hwnd, m_config->get_not_apply_style_to_classnames());
            m_workspace_it->unmanage(*it);
            m_workspace_it->manage_back(*it, m_config->get_not_apply_style_to_classnames());
            it++;
          }
        }
        else{
          m_workspace_it->unmanage(*it);
          m_workspace_it->manage_back(*it, m_config->get_not_apply_style_to_classnames());
        }
      }
    }
  }
  void TilingWindowManager::run_process(){
    auto const path = m_config->get_run_process_path();
    if(path){
      ::ShellExecute(NULL, NULL, path->c_str(), "", NULL, SW_SHOWDEFAULT);
    }
  }
  void TilingWindowManager::exit_tile(){
    ::PostMessage(m_main_hwnd, WM_CLOSE, 0, 0);
  }
  void TilingWindowManager::kill_client(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    ::PostMessage(foreground_hwnd, WM_CLOSE, 0, 0);
  }
  void TilingWindowManager::next_layout(){
    m_workspace_it->next_layout();
    arrange();
    try_focus_managed_window();

    std::stringstream ss;
    ss << "<next_layout>" << std::endl << m_workspace_it->get_layout_name();
    set_toast_text(ss.str(), 240, 100);
  }
  void TilingWindowManager::next_focus(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    auto const classnames = m_config->get_ignore_classnames_arranged().value;
    std::deque<HWND> const hwnds = m_workspace_it->get_managed_hwnds();
    if(0 < hwnds.size()){
      bool is_next_focus = false;
      if(std::find(std::begin(hwnds), std::end(hwnds), foreground_hwnd) != std::end(hwnds)){
        for(auto it = hwnds.begin(); it < hwnds.end(); it++){
          if(std::find(std::begin(classnames), std::end(classnames), get_classname(*it)) != std::end(classnames)){
            continue;
          }
          if(is_next_focus){
            ::SetForegroundWindow(*it);
            is_next_focus = false;
            break;
          }
          else if(foreground_hwnd == *it){
            is_next_focus = true;
          }
        }
      }
      else{
        is_next_focus = true;
      }
      if(is_next_focus){
        for(auto it = hwnds.begin(); it < hwnds.end(); it++){
          if(std::find(std::begin(classnames), std::end(classnames), get_classname(*it)) == std::end(classnames)){
            ::SetForegroundWindow(*it);
            break;
          }
        }
      }
    }
  }
  void TilingWindowManager::previous_focus(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    auto const classnames = m_config->get_ignore_classnames_arranged().value;
    std::deque<HWND> const hwnds = m_workspace_it->get_managed_hwnds();
    if(0 < hwnds.size()){
      bool is_next_focus = false;
      if(std::find(std::begin(hwnds), std::end(hwnds), foreground_hwnd) != std::end(hwnds)){
        for(auto it = hwnds.rbegin(); it < hwnds.rend(); it++){
          if(std::find(std::begin(classnames), std::end(classnames), get_classname(*it)) != std::end(classnames)){
            continue;
          }
          if(is_next_focus){
            ::SetForegroundWindow(*it);
            is_next_focus = false;
            break;
          }
          else if(foreground_hwnd == *it){
            is_next_focus = true;
          }
        }
      }
      else{
        is_next_focus = true;
      }
      if(is_next_focus){
        for(auto it = hwnds.rbegin(); it < hwnds.rend(); it++){
          if(std::find(std::begin(classnames), std::end(classnames), get_classname(*it)) == std::end(classnames)){
            ::SetForegroundWindow(*it);
            break;
          }
        }
      }
    }
  }
  std::string TilingWindowManager::get_toast_text() const{
    return m_toast_text;
  }
  void TilingWindowManager::set_toast_text(std::string const& text, unsigned int const width, unsigned int const height){
    m_toast_text = text;
    ::SendMessage(m_toast_hwnd, WM_TOAST, 0, 0);
    RECT const rect = get_window_area();
    ::SetWindowPos(m_toast_hwnd, HWND_TOPMOST,
        (rect.left + (rect.right - width) / 2),
        (rect.top + (rect.bottom - height) / 2),
        (width),
        (height),
        SWP_NOACTIVATE);
  }
  template <unsigned int i>
    void TilingWindowManager::move_to_workspace(){
      HWND const hwnd = ::GetForegroundWindow();
      unsigned int n = 0;
      for(auto it = std::begin(m_workspaces); it < std::end(m_workspaces); it++){
        if(n == i && it != m_workspace_it){
          m_workspace_it->unmanage(hwnd);
          it->manage_front(hwnd, m_config->get_not_apply_style_to_classnames());
          arrange();
          try_focus_managed_window();
          ::ShowWindow(hwnd, SW_HIDE);
          break;
        }
        n++;
      }

      std::stringstream ss;
      ss << "<move_to_workspace>" << std::endl << (i + 1);
      set_toast_text(ss.str(), 240, 100);
    }
  template <unsigned int i>
    void TilingWindowManager::workspace(){
      std::stringstream ss;
      ss << "<workspace>" << std::endl << (i + 1);
      set_toast_text(ss.str(), 240, 100);
      unsigned int n = 0;
      if(i < m_workspaces.size()){
        for(auto it = std::begin(m_workspaces); it < std::end(m_workspaces); it++){
          if(n == i){
            if(m_workspace_it != it){
              for(auto hwnd : m_workspace_it->get_managed_hwnds()){
                ::ShowWindow(hwnd, SW_HIDE);
              }
              m_workspace_it = it;
              auto const hwnds = m_workspace_it->get_managed_hwnds();
              for(auto hwnd : hwnds){
                m_workspace_it->unmanage(hwnd);
                m_workspace_it->manage_back(hwnd, m_config->get_not_apply_style_to_classnames());
              }
              arrange();
              try_focus_managed_window();
              for(auto hwnd : m_workspace_it->get_managed_hwnds()){
                ::ShowWindow(hwnd, SW_SHOWNORMAL);
              }
            }
            break;
          }
          n++;
        }
      }
    }
  void TilingWindowManager::scrollwheel_up(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    RECT r;
    ::GetWindowRect(foreground_hwnd, &r);
    unsigned int const x = r.top + (r.bottom - r.top) / 2;
    unsigned int const y = r.left + (r.right - r.left) / 2;
    ::SendMessage(foreground_hwnd, WM_MOUSEWHEEL, MAKEWPARAM(0, WHEEL_DELTA * 1), MAKELPARAM(y, x));
  }
  void TilingWindowManager::scrollwheel_down(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    RECT r;
    ::GetWindowRect(foreground_hwnd, &r);
    unsigned int const x = r.top + (r.bottom - r.top) / 2;
    unsigned int const y = r.left + (r.right - r.left) / 2;
    ::SendMessage(foreground_hwnd, WM_MOUSEWHEEL, MAKEWPARAM(0, WHEEL_DELTA * -1), MAKELPARAM(y, x));
  }
  void TilingWindowManager::focus_window_to_master(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    if(0 < m_workspace_it->count()){
      m_workspace_it->unmanage(foreground_hwnd);
      m_workspace_it->manage_front(foreground_hwnd, m_config->get_not_apply_style_to_classnames());
      arrange();
      try_focus_managed_window();
    }

    set_toast_text("<focus_window_to_master>", 240, 100);
  }

  void TilingWindowManager::try_focus_managed_window(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    for(auto hwnd : m_workspace_it->get_managed_hwnds()){
      if(foreground_hwnd == hwnd){
        return;
      }
    }
    if(0 < m_workspace_it->count()){
      ::SetForegroundWindow(m_workspace_it->at(0));
      return;
    }
    ::SetForegroundWindow(::FindWindow("Progman", NULL));
  }
  TilingWindowManager::TilingWindowManager(HINSTANCE const& hInstance_, std::string const& main_classname_, std::shared_ptr<std::vector<Tile::Layout>> const& layouts_, std::shared_ptr<ConfigReader> const& config_) : m_hInstance(hInstance_), m_config(config_){

    m_workspaces = {
      Tile::Workspace("A", layouts_),
      Tile::Workspace("B", layouts_),
      Tile::Workspace("C", layouts_),
      Tile::Workspace("D", layouts_),
      Tile::Workspace("E", layouts_),
      Tile::Workspace("F", layouts_),
      Tile::Workspace("G", layouts_),
      Tile::Workspace("H", layouts_),
      Tile::Workspace("I", layouts_),
    };
    m_workspace_it = std::begin(m_workspaces);

    m_main_class_name = main_classname_;
    init_main();
    m_border_class_name = "TileBorder";
    init_toast();
    init_border();

    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.hWnd = m_main_hwnd;
    m_nid.hIcon = ::LoadIcon(hInstance_, "SAMPLEICON");
    lstrcpy( m_nid.szTip, "Tile");
    ::Shell_NotifyIcon( NIM_ADD, &m_nid );

    m_shellhookid = ::RegisterWindowMessage("SHELLHOOK");
  }
  TilingWindowManager::~TilingWindowManager(){
    ::Shell_NotifyIcon( NIM_DELETE, &m_nid );

    unmanage_all();
    if (!m_main_hwnd){
      ::DestroyWindow(m_main_hwnd);
    }
    if (!m_toast_hwnd){
      ::DestroyWindow(m_toast_hwnd);
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
      ::UnregisterClass(m_main_class_name.c_str(), m_hInstance);
      ::UnregisterClass(m_border_class_name.c_str(), m_hInstance);
    }
  }
  UINT const TilingWindowManager::start(){
    typedef BOOL (* RegisterShellHookWindowProc) (HWND);
    RegisterShellHookWindowProc RegisterShellHookWindow \
      = reinterpret_cast<RegisterShellHookWindowProc>(::GetProcAddress( ::GetModuleHandle("USER32.DLL"), "RegisterShellHookWindow"));
    if (!RegisterShellHookWindow){
      die("Could not find RegisterShellHookWindow");
    }

    RegisterShellHookWindow(m_main_hwnd);

    regist_key("exit_tile", &TilingWindowManager::exit_tile);
    regist_key("focus_window_to_master", &TilingWindowManager::focus_window_to_master);
    regist_key("kill_client", &TilingWindowManager::kill_client);
    regist_key("move_to_workspace_1", &TilingWindowManager::move_to_workspace<0>);
    regist_key("move_to_workspace_2", &TilingWindowManager::move_to_workspace<1>);
    regist_key("move_to_workspace_3", &TilingWindowManager::move_to_workspace<2>);
    regist_key("move_to_workspace_4", &TilingWindowManager::move_to_workspace<3>);
    regist_key("move_to_workspace_5", &TilingWindowManager::move_to_workspace<4>);
    regist_key("move_to_workspace_6", &TilingWindowManager::move_to_workspace<5>);
    regist_key("move_to_workspace_7", &TilingWindowManager::move_to_workspace<6>);
    regist_key("move_to_workspace_8", &TilingWindowManager::move_to_workspace<7>);
    regist_key("move_to_workspace_9", &TilingWindowManager::move_to_workspace<8>);
    regist_key("next_focus", &TilingWindowManager::next_focus);
    regist_key("next_layout", &TilingWindowManager::next_layout);
    regist_key("previous_focus", &TilingWindowManager::previous_focus);
    regist_key("rescan", &TilingWindowManager::rescan);
    regist_key("show_information", &TilingWindowManager::show_information);
    regist_key("toggle_border", &TilingWindowManager::toggle_border);
    regist_key("toggle_transparency_window", &TilingWindowManager::toggle_transparency_window);
    regist_key("swap_next", &TilingWindowManager::swap_next);
    regist_key("swap_previous", &TilingWindowManager::swap_previous);
    regist_key("run_process", &TilingWindowManager::run_process);
    regist_key("workspace_1", &TilingWindowManager::workspace<0>);
    regist_key("workspace_2", &TilingWindowManager::workspace<1>);
    regist_key("workspace_3", &TilingWindowManager::workspace<2>);
    regist_key("workspace_4", &TilingWindowManager::workspace<3>);
    regist_key("workspace_5", &TilingWindowManager::workspace<4>);
    regist_key("workspace_6", &TilingWindowManager::workspace<5>);
    regist_key("workspace_7", &TilingWindowManager::workspace<6>);
    regist_key("workspace_8", &TilingWindowManager::workspace<7>);
    regist_key("workspace_9", &TilingWindowManager::workspace<8>);
    regist_key("scrollwheel_up", &TilingWindowManager::scrollwheel_up);
    regist_key("scrollwheel_down", &TilingWindowManager::scrollwheel_down);

    ::EnumWindows(scan, 0);
    arrange();
    try_focus_managed_window();

    ::MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0) > 0){
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }

    ::DeregisterShellHookWindow(m_main_hwnd);

    return msg.wParam;
  }
  void TilingWindowManager::arrange(){
    m_workspace_it->arrange(m_config->get_ignore_classnames_arranged());
    unsigned int const n = 3;
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
  }
  void TilingWindowManager::manage(HWND const& hwnd_){
    if(is_manageable(hwnd_)){
#ifdef DEBUG
      HWND const parent = ::GetParent(hwnd_);
      std::string const classname = get_classname(hwnd_);
      std::cout << "[TilingWindowManager::manage]" << std::endl;
      std::cout << "hwnd:" << hwnd_ << std::endl;
      std::cout << "parent:" << parent << std::endl;
      std::cout << "classname:" << classname << std::endl;
      std::cout << std::endl;
#endif
      recovery.save(hwnd_);
      m_workspace_it->manage_front(hwnd_, m_config->get_not_apply_style_to_classnames());
      arrange();
    }
  }
  void TilingWindowManager::unmanage(HWND const& hwnd_){
    m_workspace_it->unmanage(hwnd_);
  }
  void TilingWindowManager::call_key_method(UINT const& i_){
    try{
      for(auto key : m_keys){
        if(key->hash() == i_){
          key->call();
        }
      }
    }
    catch(...){
      system_error("TilingWindowManager::call_key_method");
      exit_tile();
    }
  }
}



#include "./common_headers.h"
#include "./common_functions.h"
#include "./wndproc_functions.h"
#include "./tile.h"

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
  void TilingWindowManager::init_statusline(){
    WNDCLASSEX winClass = make_wndclassex(m_hInstance, StatusLineWndProc, m_statusline_class_name);
    ATOM atom = ::RegisterClassEx(&winClass);
    if(!atom){
      die("Error registering window class(statusline)");
    }

    m_statusline_hwnd = make_toolwindow(m_hInstance, m_statusline_class_name);
    ::SetWindowPos(m_statusline_hwnd, HWND_TOPMOST,
        0, 0, get_statusline_width(), get_statusline_height(), SWP_NOACTIVATE);
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
  }
  bool TilingWindowManager::is_manageable(HWND const hwnd_){
    if(hwnd_ == NULL){
      return false;
    }

    HWND const parent = ::GetParent(hwnd_);
    int const style = ::GetWindowLong(hwnd_, GWL_STYLE);
    int const exstyle = ::GetWindowLong(hwnd_, GWL_EXSTYLE);
    bool const pok = (parent != 0 && is_manageable_arrange(parent));
    bool const istool = exstyle & WS_EX_TOOLWINDOW;
    bool const isapp = exstyle & WS_EX_APPWINDOW;

    std::string const classname = get_classname(hwnd_);
    if(classname == m_statusline_class_name){
      return false;
    }

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
  bool TilingWindowManager::is_manageable_focus(HWND const hwnd_){
    return is_manageable(hwnd_);
  }
  bool TilingWindowManager::is_manageable_arrange(HWND const hwnd_){
    std::string const classname = get_classname(hwnd_);
    for(auto c : m_config->get_ignore_classnames()){
      if(classname == c){
        return false;
      }
    }
    return is_manageable(hwnd_);
  }
  void TilingWindowManager::regist_key(std::string key, void (Tile::TilingWindowManager::* f_)()){
    std::map<std::string, std::string> m = m_config->get_keys();
    auto it = m.find(key);
    if(it != std::end(m)){
      unsigned long MODKEY = MOD_ALT | MOD_CONTROL;
      unsigned char n = std::atoi(m[key].c_str());
      switch(n){
        case '!': n = '1'; MODKEY |= MOD_SHIFT; break;
        case '@': n = '2'; MODKEY |= MOD_SHIFT; break;
        case '#': n = '3'; MODKEY |= MOD_SHIFT; break;
        case '$': n = '4'; MODKEY |= MOD_SHIFT; break;
        case '%': n = '5'; MODKEY |= MOD_SHIFT; break;
        case '^': n = '6'; MODKEY |= MOD_SHIFT; break;
        case '&': n = '7'; MODKEY |= MOD_SHIFT; break;
        case '*': n = '8'; MODKEY |= MOD_SHIFT; break;
        case '(': n = '9'; MODKEY |= MOD_SHIFT; break;
      }
      m_keys.push_back(std::shared_ptr<Key>(new Key(m_main_hwnd, MODKEY, n, std::bind(f_, this))));
    }
  }
  void TilingWindowManager::unmanage_all(){
    for(auto it = std::begin(m_workspaces); it < std::end(m_workspaces); it++){
      for(auto hwnd : it->get_managed_hwnds()){
        it->unmanage(hwnd);
        ::ShowWindow(hwnd, SW_SHOWNORMAL);
      }
    }
  }
  void TilingWindowManager::rescan(){
    unmanage_all();
    ::EnumWindows(scan, 0);
    arrange();
  }
  void TilingWindowManager::run_shell(){
    ::ShellExecute(NULL, NULL, "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe", "", NULL, SW_SHOWDEFAULT);
  }
  void TilingWindowManager::exit_tile(){
    ::PostMessage(m_main_hwnd, WM_CLOSE, 0, 0);
  }
  void TilingWindowManager::kill_client(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    ::PostMessage(foreground_hwnd, WM_CLOSE, 0, 0);
  }
  void TilingWindowManager::next_layout(){
    m_layout_it++;
    if(m_layout_it == std::end(m_layouts)){
      m_layout_it = std::begin(m_layouts);
    }
    arrange();
  }
  void TilingWindowManager::next_focus(){
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
  void TilingWindowManager::previous_focus(){
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
  void TilingWindowManager::move_to_workspace_1(){
    move_to_workspace_of(0);
  }
  void TilingWindowManager::move_to_workspace_2(){
    move_to_workspace_of(1);
  }
  void TilingWindowManager::move_to_workspace_3(){
    move_to_workspace_of(2);
  }
  void TilingWindowManager::move_to_workspace_4(){
    move_to_workspace_of(3);
  }
  void TilingWindowManager::move_to_workspace_5(){
    move_to_workspace_of(4);
  }
  void TilingWindowManager::move_to_workspace_6(){
    move_to_workspace_of(5);
  }
  void TilingWindowManager::move_to_workspace_7(){
    move_to_workspace_of(6);
  }
  void TilingWindowManager::move_to_workspace_8(){
    move_to_workspace_of(7);
  }
  void TilingWindowManager::move_to_workspace_9(){
    move_to_workspace_of(8);
  }
  void TilingWindowManager::workspace_1(){
    workspace_of(0);
  }
  void TilingWindowManager::workspace_2(){
    workspace_of(1);
  }
  void TilingWindowManager::workspace_3(){
    workspace_of(2);
  }
  void TilingWindowManager::workspace_4(){
    workspace_of(3);
  }
  void TilingWindowManager::workspace_5(){
    workspace_of(4);
  }
  void TilingWindowManager::workspace_6(){
    workspace_of(5);
  }
  void TilingWindowManager::workspace_7(){
    workspace_of(6);
  }
  void TilingWindowManager::workspace_8(){
    workspace_of(7);
  }
  void TilingWindowManager::workspace_9(){
    workspace_of(8);
  }
  void TilingWindowManager::focus_window_to_master(){
    HWND const foreground_hwnd = ::GetForegroundWindow();
    if(0 < m_workspace_it->size()){
      m_workspace_it->remanage(foreground_hwnd, m_config->get_not_apply_style_to_classnames());
      arrange();
    }
  }
  void TilingWindowManager::move_to_workspace_of(unsigned int const i){
    HWND const hwnd = ::GetForegroundWindow();
    if(is_manageable_arrange(hwnd) || is_manageable_focus(hwnd)){
      unsigned int n = 0;
      for(auto it = std::begin(m_workspaces); it < std::end(m_workspaces); it++){
        if(n == i && it != m_workspace_it){
          m_workspace_it->unmanage(hwnd);
          it->manage(hwnd, m_config->get_not_apply_style_to_classnames());
          arrange();
          ::ShowWindow(hwnd, SW_HIDE);
          ::SetForegroundWindow(::FindWindow("Progman", NULL));
          break;
        }
        n++;
      }
    }
  }
  void TilingWindowManager::workspace_of(unsigned int const i){
    unsigned int n = 0;
    if(i < m_workspaces.size()){
      for(auto it = std::begin(m_workspaces); it < std::end(m_workspaces); it++){
        if(n == i){
          if(m_workspace_it != it){
            for(auto hwnd : m_workspace_it->get_managed_hwnds()){
              ::ShowWindow(hwnd, SW_HIDE);
            }
            m_workspace_it = it;
            for(auto hwnd : m_workspace_it->get_managed_hwnds()){
              m_workspace_it->remanage(hwnd, m_config->get_not_apply_style_to_classnames());
            }
            arrange();
          }
          break;
        }
        n++;
      }
    }
  }
  TilingWindowManager::TilingWindowManager(HINSTANCE const& hInstance_, std::string main_classname_, std::vector<Tile::Layout> layouts_, std::shared_ptr<ConfigReader> config_){
    m_hInstance = hInstance_;

    m_config = config_;

    m_workspaces = {
      Tile::Workspace("1"),
      Tile::Workspace("2"),
      Tile::Workspace("3"),
      Tile::Workspace("4"),
      Tile::Workspace("5"),
      Tile::Workspace("6"),
      Tile::Workspace("7"),
      Tile::Workspace("8"),
      Tile::Workspace("9"),
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
  TilingWindowManager::~TilingWindowManager(){
    unmanage_all();
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
      ::UnregisterClass(m_main_class_name.c_str(), m_hInstance);
      ::UnregisterClass(m_statusline_class_name.c_str(), m_hInstance);
      ::UnregisterClass(m_border_class_name.c_str(), m_hInstance);
    }
  }
  UINT const TilingWindowManager::start(){
    typedef BOOL (* RegisterShellHookWindowProc) (HWND);
    RegisterShellHookWindowProc RegisterShellHookWindow \
      = (RegisterShellHookWindowProc) ::GetProcAddress( ::GetModuleHandle("USER32.DLL"), "RegisterShellHookWindow");
    if (!RegisterShellHookWindow){
      die("Could not find RegisterShellHookWindow");
    }

    RegisterShellHookWindow(m_main_hwnd);

    regist_key("exit_tile", &TilingWindowManager::exit_tile);
    regist_key("focus_window_to_master", &TilingWindowManager::focus_window_to_master);
    regist_key("kill_client", &TilingWindowManager::kill_client);
    regist_key("move_to_workspace_1", &TilingWindowManager::move_to_workspace_1);
    regist_key("move_to_workspace_2", &TilingWindowManager::move_to_workspace_2);
    regist_key("move_to_workspace_3", &TilingWindowManager::move_to_workspace_3);
    regist_key("move_to_workspace_4", &TilingWindowManager::move_to_workspace_4);
    regist_key("move_to_workspace_5", &TilingWindowManager::move_to_workspace_5);
    regist_key("move_to_workspace_6", &TilingWindowManager::move_to_workspace_6);
    regist_key("move_to_workspace_7", &TilingWindowManager::move_to_workspace_7);
    regist_key("move_to_workspace_8", &TilingWindowManager::move_to_workspace_8);
    regist_key("move_to_workspace_9", &TilingWindowManager::move_to_workspace_9);
    regist_key("next_focus", &TilingWindowManager::next_focus);
    regist_key("next_layout", &TilingWindowManager::next_layout);
    regist_key("previous_focus", &TilingWindowManager::previous_focus);
    regist_key("rescan", &TilingWindowManager::rescan);
    regist_key("run_shell", &TilingWindowManager::run_shell);
    regist_key("workspace_1", &TilingWindowManager::workspace_1);
    regist_key("workspace_2", &TilingWindowManager::workspace_2);
    regist_key("workspace_3", &TilingWindowManager::workspace_3);
    regist_key("workspace_4", &TilingWindowManager::workspace_4);
    regist_key("workspace_5", &TilingWindowManager::workspace_5);
    regist_key("workspace_6", &TilingWindowManager::workspace_6);
    regist_key("workspace_7", &TilingWindowManager::workspace_7);
    regist_key("workspace_8", &TilingWindowManager::workspace_8);
    regist_key("workspace_9", &TilingWindowManager::workspace_9);

    ::EnumWindows(scan, 0);
    arrange();

    ::MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0) > 0){
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }

    ::DeregisterShellHookWindow(m_main_hwnd);

    return msg.wParam;
  }
  void TilingWindowManager::arrange(){
    if(m_layout_it != std::end(m_layouts)){
      std::deque<HWND> hwnds;
      for(auto hwnd : m_workspace_it->get_managed_hwnds()){
        if(is_manageable_arrange(hwnd)){
          hwnds.push_back(hwnd);
        }
      }
      m_layout_it->arrange(hwnds);
    }
    if(is_manageable_arrange(::GetForegroundWindow()) || is_manageable_focus(::GetForegroundWindow())){
      unsigned int n = 3;
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
  void TilingWindowManager::manage(HWND hwnd_){
    if(is_manageable_arrange(hwnd_) || is_manageable_focus(hwnd_)){

#ifdef DEBUG
      HWND const parent = ::GetParent(hwnd_);
      std::string const classname = get_classname(hwnd_);
      std::cout << "hwnd:" << hwnd_ << std::endl;
      std::cout << "parent:" << parent << std::endl;
      std::cout << "classname:" << classname << std::endl;
      std::cout << std::endl;
#endif

      m_workspace_it->manage(hwnd_, m_config->get_not_apply_style_to_classnames());
    }
  }
  void TilingWindowManager::unmanage(HWND hwnd_){
    m_workspace_it->unmanage(hwnd_);
  }
  void TilingWindowManager::call_key_method(UINT const& i_) const{
    for(auto key : m_keys){
      if(key->hash() == i_){
        key->call();
      }
    }
  }
  void TilingWindowManager::redraw_statusline(){
    ::PostMessage(m_statusline_hwnd, WM_PAINT, 0, 0);
  }
  std::string TilingWindowManager::get_layout_name(){
    if(0 < m_layouts.size()){
      return m_layout_it->get_layout_name();
    }
    else{
      return "";
    }
  }
  std::string TilingWindowManager::get_workspace_name() const{
    return m_workspace_it->get_workspace_name();
  }
  long TilingWindowManager::get_managed_window_size() const{
    return m_workspace_it->size();
  }

  void Workspace::set_style(HWND hwnd_, std::vector<std::string> classnames_){
    std::string const classname = get_classname(hwnd_);
    LONG const style = ::GetWindowLong(hwnd_, GWL_STYLE);
    bool b = true;
    for(auto c : classnames_){
      if(classname == c){
        b = false;
      }
    }
    if(b){
      ::SetWindowLong(hwnd_, GWL_STYLE, style ^ WS_CAPTION ^ WS_THICKFRAME );
    }
  }
  Workspace::Workspace(std::string workspace_name_): m_workspace_name(workspace_name_){
  }
  unsigned int Workspace::size(){
    return m_managed_hwnds.size();
  }
  HWND Workspace::at(unsigned int i){
    return m_managed_hwnds.at(i);
  }
  std::deque<HWND>::iterator Workspace::begin(){
    return std::begin(m_managed_hwnds);
  }
  std::deque<HWND>::iterator Workspace::end(){
    return std::begin(m_managed_hwnds);
  }
  std::deque<HWND>::reverse_iterator Workspace::rbegin(){
    return m_managed_hwnds.rbegin();
  }
  std::deque<HWND>::reverse_iterator Workspace::rend(){
    return m_managed_hwnds.rend();
  }
  bool Workspace::is_managed(HWND hwnd_){
    auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    return (it != std::end(m_managed_hwnds));
  }
  void Workspace::remanage(HWND hwnd_, std::vector<std::string> classnames_){
    unmanage(hwnd_);
    manage(hwnd_, classnames_);
  }
  void Workspace::manage(HWND hwnd_, std::vector<std::string> classnames_){
    if(!is_managed(hwnd_)){
      RECT rect;
      ::GetWindowRect(hwnd_, &rect);
      m_managed_hwnds.push_front(hwnd_);
      m_managed_hwnd_to_rect.insert(std::map<HWND, RECT>::value_type(hwnd_, rect));
      m_managed_hwnd_to_style.insert(std::map<HWND, LONG>::value_type(hwnd_, ::GetWindowLong(hwnd_, GWL_STYLE)));
      m_managed_hwnd_to_exstyle.insert(std::map<HWND, LONG>::value_type(hwnd_, ::GetWindowLong(hwnd_, GWL_EXSTYLE)));
      set_style(hwnd_, classnames_);

      // ::SetWindowLong(hwnd_, GWL_EXSTYLE, m_managed_hwnd_to_exstyle[hwnd_] | WS_EX_LAYERED );
      // ::SetLayeredWindowAttributes(hwnd_, 0, 200, LWA_ALPHA);
    }
  }
  void Workspace::unmanage(HWND const hwnd_){
    auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    if(it != std::end(m_managed_hwnds)){
      m_managed_hwnds.erase(it);
      RECT const rect = m_managed_hwnd_to_rect[hwnd_];
      LONG const width = rect.right - rect.left;
      LONG const height = rect.bottom - rect.top;
      ::SetWindowPos(hwnd_, HWND_TOP, rect.left, rect.top, width, height, SWP_NOACTIVATE);
      ::SetWindowLong(hwnd_, GWL_STYLE, m_managed_hwnd_to_style[hwnd_]);
      ::SetWindowLong(hwnd_, GWL_EXSTYLE, m_managed_hwnd_to_exstyle[hwnd_]);
    }
  }
  std::deque<HWND> const& Workspace::get_managed_hwnds() const{
    return m_managed_hwnds;
  }
  std::string Workspace::get_workspace_name() const{
    return m_workspace_name;
  }

  Key::Key(HWND const& hwnd_, UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_)
    : m_mod(mod_), m_k(k_), m_hash_value((m_mod * 100) + m_k), m_f(f_), m_main_hwnd(hwnd_) {
      ::RegisterHotKey(m_main_hwnd, m_hash_value, m_mod, m_k);
    }
  Key::~Key(){
    ::UnregisterHotKey(m_main_hwnd, m_hash_value);
  }
  UINT Key::hash() const{
    return m_hash_value;
  }
  void Key::call() const{
    m_f();
  }

  std::vector<std::string> ConfigReader::split(const std::string& input_, char delimiter_){
    std::istringstream stream(input_);
    std::string field;
    std::vector<std::string> result;
    while (std::getline(stream, field, delimiter_)) {
      result.push_back(field);
    }
    return result;
  }
  std::vector<std::string> ConfigReader::get_config_settings_ignore_classnames(){
    char buffer[buffer_size];
    ::GetPrivateProfileString("settings", "IGNORE_CLASSNAMES", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
    return split(std::string(buffer), ',');
  }
  std::vector<std::string> ConfigReader::get_config_settings_not_apply_style_to_classnames(){
    char buffer[buffer_size];
    ::GetPrivateProfileString("settings", "NOT_APPLY_STYLE_TO_CLASSNAMES", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
    return split(std::string(buffer), ',');
  }
  std::map<std::string, std::string> ConfigReader::get_config_keys(){
    std::map<std::string, std::string> m;
    char buffer[buffer_size];
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
  std::string ConfigReader::get_inifile_path(){
    char szFileName[buffer_size];
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
  ConfigReader::ConfigReader(){
    reload();
  }
  void ConfigReader::reload(){
    m_ignore_classnames = get_config_settings_ignore_classnames();
    m_not_apply_style_to_classnames = get_config_settings_not_apply_style_to_classnames();
    m_keys = get_config_keys();
    m_inifile_path = get_inifile_path();
  }
  std::vector<std::string> const& ConfigReader::get_ignore_classnames() const{
    return m_ignore_classnames;
  }
  std::vector<std::string> const& ConfigReader::get_not_apply_style_to_classnames() const{
    return m_not_apply_style_to_classnames;
  }
  std::map<std::string, std::string> const& ConfigReader::get_keys() const{
    return m_keys;
  }
  std::string const& ConfigReader::get_inipath() const{
    return m_inifile_path;
  }

  Layout::Layout(std::string layout_name_, std::function<void(std::deque<HWND> const& hwnds_)> f_) : m_layout_name(layout_name_), m_f(f_){
  }
  std::string Layout::get_layout_name() const{
    return m_layout_name;
  }
  void Layout::arrange(std::deque<HWND> const& hwnds_){
    m_f(hwnds_);
  }
}


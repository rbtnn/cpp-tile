
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./Workspace.h"

namespace Tile{
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
  void Workspace::remanage_back(HWND hwnd_){
    auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    if(it != std::end(m_managed_hwnds)){
      m_managed_hwnds.erase(it);
    }
    if(!is_managed(hwnd_)){
      m_managed_hwnds.push_back(hwnd_);
    }
  }
  void Workspace::remanage_front(HWND hwnd_){
    auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    if(it != std::end(m_managed_hwnds)){
      m_managed_hwnds.erase(it);
    }
    if(!is_managed(hwnd_)){
      m_managed_hwnds.push_front(hwnd_);
    }
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
}


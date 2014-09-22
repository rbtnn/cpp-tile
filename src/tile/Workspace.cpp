
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./Workspace.h"

namespace Tile{
  void Workspace::set_style(HWND const& hwnd_, std::vector<std::string> const& classnames_){
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
  Workspace::Workspace(std::string const& workspace_name_, std::shared_ptr<std::vector<Tile::Layout>> const& layouts_): m_workspace_name(workspace_name_){
    m_layouts = layouts_;
    m_layout_it = std::begin(*m_layouts);
  }
  unsigned int Workspace::size(){
    return m_managed_hwnds.size();
  }
  HWND Workspace::at(unsigned int const& i_){
    return m_managed_hwnds.at(i_);
  }
  std::deque<HWND>::reverse_iterator const Workspace::rbegin(){
    return m_managed_hwnds.rbegin();
  }
  std::deque<HWND>::reverse_iterator const Workspace::rend(){
    return m_managed_hwnds.rend();
  }
  bool Workspace::is_managed(HWND const& hwnd_){
    auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    return (it != std::end(m_managed_hwnds));
  }
  void Workspace::remanage_back(HWND const& hwnd_){
    auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    if(it != std::end(m_managed_hwnds)){
      m_managed_hwnds.erase(it);
    }
    if(!is_managed(hwnd_)){
      m_managed_hwnds.push_back(hwnd_);
    }
  }
  void Workspace::remanage_front(HWND const& hwnd_){
    auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    if(it != std::end(m_managed_hwnds)){
      m_managed_hwnds.erase(it);
    }
    if(!is_managed(hwnd_)){
      m_managed_hwnds.push_front(hwnd_);
    }
  }
  void Workspace::manage(HWND const& hwnd_, std::vector<std::string> const& classnames_){
    if(!is_managed(hwnd_)){
      m_managed_hwnds.push_front(hwnd_);
      set_style(hwnd_, classnames_);
    }
  }
  void Workspace::unmanage(HWND const& hwnd_){
    auto it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    if(it != std::end(m_managed_hwnds)){
      m_managed_hwnds.erase(it);
    }
  }
  std::deque<HWND> const& Workspace::get_managed_hwnds() const{
    return m_managed_hwnds;
  }
  std::string Workspace::get_workspace_name() const{
    return m_workspace_name;
  }
  void Workspace::next_layout(){
    m_layout_it++;
    if(m_layout_it == std::end(*m_layouts)){
      m_layout_it = std::begin(*m_layouts);
    }
  }
  boost::optional<std::string> Workspace::get_layout_name() const{
    if(0 < m_layouts->size()){
      return m_layout_it->get_layout_name();
    }
    else{
      return boost::none;
    }
  }
  void Workspace::arrange(std::vector<std::string> const& classnames_){
    if(m_layout_it != std::end(*m_layouts)){
      std::deque<HWND> hwnds;
      for(auto hwnd : m_managed_hwnds){
        if(is_manageable(hwnd)){
          bool b = true;
          std::string const classname = get_classname(hwnd);
          for(auto c : classnames_){
            if(classname == c){
              b = false;
            }
          }
          if(b){
            hwnds.push_back(hwnd);
          }
        }
      }
      m_layout_it->arrange(hwnds);
    }
  }
}



#include <tile/common_headers.h>
#include <tile/common_functions.h>
#include <tile/wndproc_functions.h>
#include <tile/Workspace.h>
#include <tile/IgnoreClassNamesArranged.h>
#include <tile/NotApplyStyleToClassNames.h>

namespace Tile{
  void Workspace::set_style(HWND const& hwnd_, Tile::NotApplyStyleToClassNames const& classnames_){
    std::string const classname = get_classname(hwnd_);
    LONG const style = ::GetWindowLong(hwnd_, GWL_STYLE);
    bool b = true;
    for(auto c : classnames_.value){
      if(classname == c){
        b = false;
      }
    }
    if(b){
      ::SetWindowLong(hwnd_, GWL_STYLE, style ^ WS_CAPTION ^ WS_THICKFRAME);
    }
  }
  Workspace::Workspace(std::string const& workspace_name_, std::shared_ptr<std::vector<Tile::Layout>> const& layouts_): m_workspace_name(workspace_name_){
    m_layouts = layouts_;
    m_layout_it = std::begin(*m_layouts);
  }
  unsigned int Workspace::count(){
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
  void Workspace::manage_back(HWND const& hwnd_, Tile::NotApplyStyleToClassNames const& classnames_){
    auto const it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    if(it == std::end(m_managed_hwnds)){
      m_managed_hwnds.push_back(hwnd_);
      set_style(hwnd_, classnames_);
    }
  }
  void Workspace::manage_front(HWND const& hwnd_, Tile::NotApplyStyleToClassNames const& classnames_){
    auto const it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
    if(it == std::end(m_managed_hwnds)){
      m_managed_hwnds.push_front(hwnd_);
      set_style(hwnd_, classnames_);
    }
  }
  void Workspace::unmanage(HWND const& hwnd_){
    auto const it = std::find(std::begin(m_managed_hwnds), std::end(m_managed_hwnds), hwnd_);
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
  void Workspace::arrange(Tile::IgnoreClassNamesArranged const& classnames_){
    if(m_layout_it != std::end(*m_layouts)){
      std::deque<HWND> hwnds;
      for(auto hwnd : m_managed_hwnds){
        if(is_manageable(hwnd)){
          auto const classnames = classnames_.value;
          if(std::find(std::begin(classnames), std::end(classnames), get_classname(hwnd)) == std::end(classnames)){
            hwnds.push_back(hwnd);
          }
        }
      }
      m_layout_it->arrange(hwnds);
    }
  }
}



#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./Layout.h"

namespace Tile{
  Layout::Layout(std::string layout_name_, std::function<void(std::deque<HWND> const& hwnds_)> f_) : m_layout_name(layout_name_), m_f(f_){
  }
  std::string Layout::get_layout_name() const{
    return m_layout_name;
  }
  void Layout::arrange(std::deque<HWND> const& hwnds_){
    m_f(hwnds_);
  }
}


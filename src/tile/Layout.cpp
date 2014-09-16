
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./Layout.h"

namespace Tile{
  Layout::Layout(std::string layout_name_, std::function<void(std::deque<HWND> const& hwnds_, long const& width_, long const& height_)> f_)
    : m_layout_name(layout_name_), m_f(f_){
  }
  std::string Layout::get_layout_name() const{
    return m_layout_name;
  }
  void Layout::arrange(std::deque<HWND> const& hwnds_){
    RECT window_area = get_window_area();
    long const width = window_area.right - window_area.left;
    long const height = window_area.bottom - window_area.top;
    m_f(hwnds_, width, height);
  }
}


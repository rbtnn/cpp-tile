
#include <tile/layout_method.h>

EXPORT void arrange_maximal(std::deque<HWND> const& hwnds_, long const& width_, long const& height_){
  HWND const foreground_hwnd = ::GetForegroundWindow();
  for(auto hwnd : hwnds_){
    resize_window(((hwnd == foreground_hwnd) ? foreground_hwnd : hwnd), HWND_NOTOPMOST, SW_SHOWNORMAL, 0, 0, width_, height_);
  }
}


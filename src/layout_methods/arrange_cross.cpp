
#include <tile/layout_method.h>

EXPORT void arrange_cross(std::deque<HWND> const& hwnds_, long const& width_, long const& height_){
  HWND const foreground_hwnd = ::GetForegroundWindow();
  auto const it = std::find(std::begin(hwnds_), std::end(hwnds_), foreground_hwnd);
  if(it != std::end(hwnds_)){
    if(0 < hwnds_.size()){
      unsigned int i = 0;
      long const w = width_ / 2;
      long const h = height_ / 2;
      for(auto hwnd : hwnds_){
        if(i == 0){
          resize_window(hwnd, HWND_NOTOPMOST, SW_SHOWNORMAL, width_ / 8 * 1, height_ / 8 * 1, width_ / 8 * 6, height_ / 8 * 6);
        }
        else{
          switch(i % 4){
            case 1: resize_window(hwnd, HWND_NOTOPMOST, SW_SHOWNORMAL, width_ / 8 * 2, height_ / 8 * 0, w, h); break;
            case 2: resize_window(hwnd, HWND_NOTOPMOST, SW_SHOWNORMAL, width_ / 8 * 4, height_ / 8 * 2, w, h); break;
            case 3: resize_window(hwnd, HWND_NOTOPMOST, SW_SHOWNORMAL, width_ / 8 * 2, height_ / 8 * 4, w, h); break;
            case 0: resize_window(hwnd, HWND_NOTOPMOST, SW_SHOWNORMAL, width_ / 8 * 0, height_ / 8 * 2, w, h); break;
          }
        }
        i++;
      }
    }
  }
}


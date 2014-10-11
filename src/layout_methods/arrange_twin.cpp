
#include <tile/layout_method.h>

EXPORT void arrange_twin(std::deque<HWND> const& hwnds_, long const& width_, long const& height_){
  if(0 == hwnds_.size()){
    // nop
  }
  else if(1 == hwnds_.size()){
    resize_window(hwnds_.at(0), HWND_NOTOPMOST, SW_SHOWNORMAL, 0, 0, width_, height_);
  }
  else if(2 == hwnds_.size()){
    resize_window(hwnds_.at(0), HWND_NOTOPMOST, SW_SHOWNORMAL, 0, 0, width_ / 2        , height_);
    resize_window(hwnds_.at(1), HWND_NOTOPMOST, SW_SHOWNORMAL, width_ / 2, 0, width_ / 2, height_);
  }
  else{
    long const n = hwnds_.size() - 2;
    long const sub_width = width_ / n;

    resize_window(hwnds_.at(0), HWND_NOTOPMOST, SW_SHOWNORMAL, 0        , 0, width_ / 2, height_ / 4 * 3);
    resize_window(hwnds_.at(1), HWND_NOTOPMOST, SW_SHOWNORMAL, width_ / 2, 0, width_ / 2, height_ / 4 * 3);

    for(unsigned int i = 2; i < hwnds_.size(); i++){
      resize_window(hwnds_.at(i), HWND_NOTOPMOST, SW_SHOWNORMAL,
          (sub_width * (i - 2)),
          (height_ / 4 * 3),
          (sub_width),
          (height_ / 4 * 1));
    }
  }
}


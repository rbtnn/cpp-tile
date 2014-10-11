
#include <tile/layout_method.h>

EXPORT void arrange(std::deque<HWND> const& hwnds_, long const& width_, long const& height_){
  long const split_size = 3;
  long sub_height = (2 < hwnds_.size()) ? (height_ / (hwnds_.size() - 1)) : height_;
  for(unsigned int i = 0; i < hwnds_.size(); i++){
    resize_window(hwnds_.at(i), HWND_NOTOPMOST, SW_SHOWNORMAL,
        ((i == 0) ? 0                                                      : width_ / split_size * 2),
        ((i == 0) ? 0                                                      : sub_height * (i - 1)),
        ((i == 0) ? ( hwnds_.size() == 1 ? width_ : width_ / split_size * 2) : width_ / split_size),
        ((i == 0) ? height_                                                 : sub_height));
  }
}



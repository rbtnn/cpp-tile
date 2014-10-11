
#include <tile/layout_method.h>

EXPORT void arrange_square(std::deque<HWND> const& hwnds_, long const& width_, long const& height_){
  unsigned long nw = 1;
  unsigned long nh = 1;
  while(nw * nh < hwnds_.size()){
    if(nh < nw){ nh++; }
    else{ nw++; }
  }
  unsigned int i = 0;
  for(unsigned int w = 0; w < nw; w++){
    for(unsigned int h = 0; h < nh; h++){
      if(i < hwnds_.size()){
        resize_window(hwnds_.at(i), HWND_NOTOPMOST, SW_SHOWNORMAL,
            (w * (width_ / nw)), (h * (height_ / nh)), (width_ / nw), (height_ / nh));
        i++;
      }
    }
  }
}



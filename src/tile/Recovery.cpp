
#include <tile/common_headers.h>
#include <tile/common_functions.h>
#include <tile/wndproc_functions.h>
#include <tile/Recovery.h>

namespace Tile{
  void Recovery::save(HWND const& hwnd_){
    auto it = rects.find(hwnd_);
    if(it == std::end(rects)){
      RECT rect;
      ::GetWindowRect(hwnd_, &rect);
      rects.insert(std::map<HWND, RECT>::value_type(hwnd_, rect));
      styles.insert(std::map<HWND, LONG>::value_type(hwnd_, ::GetWindowLong(hwnd_, GWL_STYLE)));
      exstyles.insert(std::map<HWND, LONG>::value_type(hwnd_, ::GetWindowLong(hwnd_, GWL_EXSTYLE)));
    }
  }
  void Recovery::load(HWND const& hwnd_){
    auto it = rects.find(hwnd_);
    if(it != std::end(rects)){
      RECT const rect = rects[hwnd_];
      LONG const width = rect.right - rect.left;
      LONG const height = rect.bottom - rect.top;
      ::SetWindowPos(hwnd_, HWND_NOTOPMOST, rect.left, rect.top, width, height, SWP_NOACTIVATE);
      ::SetWindowLong(hwnd_, GWL_STYLE, styles[hwnd_]);
      ::SetWindowLong(hwnd_, GWL_EXSTYLE, exstyles[hwnd_]);
      ::ShowWindow(hwnd_, SW_SHOWNORMAL);
    }
  }
}

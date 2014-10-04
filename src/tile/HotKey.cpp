
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"

#include "./HotKey.h"

namespace Tile{
  HotKey::HotKey(unsigned int const vk_, bool const shift_, bool const control_, bool const alt_, bool const win_){
    vk = vk_;
    if('a' <= vk && vk <= 'z'){
      vk -= 0x20;
    }
    fsModifiers = 0;
    if(shift_){
      fsModifiers |= MOD_SHIFT;
    }
    if(control_){
      fsModifiers |= MOD_CONTROL;
    }
    if(alt_){
      fsModifiers |= MOD_ALT;
    }
    if(win_){
      fsModifiers |= MOD_WIN;
    }
  }
  unsigned int HotKey::get_vk() const{
    return vk;
  }
  unsigned int HotKey::get_fsModifiers() const{
    return fsModifiers;
  }
}


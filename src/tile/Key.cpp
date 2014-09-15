
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./Key.h"

namespace Tile{
  Key::Key(HWND const& hwnd_, UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_)
    : m_mod(mod_), m_k(k_), m_hash_value((m_mod * 100) + m_k), m_f(f_), m_main_hwnd(hwnd_) {
      ::RegisterHotKey(m_main_hwnd, m_hash_value, m_mod, m_k);
    }
  Key::~Key(){
    ::UnregisterHotKey(m_main_hwnd, m_hash_value);
  }
  UINT Key::hash() const{
    return m_hash_value;
  }
  void Key::call() const{
    m_f();
  }
}


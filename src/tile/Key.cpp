
#include <tile/common_headers.h>
#include <tile/common_functions.h>
#include <tile/wndproc_functions.h>
#include <tile/Key.h>

namespace Tile{
  Key::Key(HWND const& hwnd_, std::string const& name_, UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_)
    : m_mod(mod_), m_k(k_), m_hash_value((m_mod << 0x16) + m_k), m_f(f_), m_main_hwnd(hwnd_) {
#ifdef DEBUG
    std::cout << "m_hash_value:" << m_hash_value << std::endl;
#endif
    m_name = name_;
    if(::RegisterHotKey(m_main_hwnd, m_hash_value, m_mod, m_k) == 0){
      system_error("failed ::RegisterHotKey()");
    }
  }
  Key::~Key(){
    ::UnregisterHotKey(m_main_hwnd, m_hash_value);
  }
  std::string Key::name() const{
    return m_name;
  }
  UINT Key::hash() const{
    return m_hash_value;
  }
  void Key::call() const{
    m_f();
  }
}


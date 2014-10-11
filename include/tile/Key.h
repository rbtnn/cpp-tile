
#ifndef TILE_KEY_H
#define TILE_KEY_H

#include "./common_headers.h"

namespace Tile{
  class Key{
    private:
      UINT const m_mod;
      UINT const m_k;
      UINT const m_hash_value;
      std::function<void(void)> const m_f;
      HWND const m_main_hwnd;

    public:
      Key(HWND const& hwnd_, UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_);
      ~Key();
      UINT hash() const;
      void call() const;
  };
}

#endif


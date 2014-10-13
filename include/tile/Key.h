
#ifndef TILE_KEY_H
#define TILE_KEY_H

#include "./common_headers.h"

namespace Tile{
  class Key{
    private:
      UINT const m_mod;
      UINT const m_k;
      std::string m_name;
      UINT const m_hash_value;
      std::function<void(void)> const m_f;
      HWND const m_main_hwnd;

    public:
      Key(HWND const&, std::string const&, UINT const&, UINT const&, std::function<void(void)> const&);
      ~Key();
      UINT hash() const;
      std::string name() const;
      void call() const;
  };
}

#endif


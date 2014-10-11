
#ifndef TILE_HOTKEY_H
#define TILE_HOTKEY_H

#include "./common_headers.h"

namespace Tile{
  class HotKey{
    private:
      unsigned int vk;
      unsigned int fsModifiers;

    public:
      HotKey(unsigned int const, bool const, bool const, bool const, bool const);
      unsigned int get_vk() const;
      unsigned int get_fsModifiers() const;
  };
}

#endif


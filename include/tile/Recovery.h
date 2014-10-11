
#ifndef TILE_RECOVERY_H
#define TILE_RECOVERY_H

#include "./common_headers.h"

namespace Tile{
  class Recovery{
    private:
      std::map<HWND, LONG> styles;
      std::map<HWND, LONG> exstyles;
      std::map<HWND, RECT> rects;

    public:
      void save(HWND const&);
      void load(HWND const&);
  };
}

#endif


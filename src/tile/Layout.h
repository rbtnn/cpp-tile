
#ifndef TILE_LAYOUT_H
#define TILE_LAYOUT_H

#include "../common_headers.h"

namespace Tile{
  class Layout{
    private:
      std::string m_layout_name;
      std::function<void(std::deque<HWND> const& hwnds_)> m_f;

    public:
      Layout(std::string, std::function<void(std::deque<HWND> const& hwnds_)>);
      std::string get_layout_name() const;
      void arrange(std::deque<HWND> const&);
  };
}

#endif


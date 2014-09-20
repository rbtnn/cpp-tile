
#ifndef TILE_LAYOUT_H
#define TILE_LAYOUT_H

#include "../common_headers.h"

namespace Tile{
  class Layout{
    public:
      typedef std::function<void(std::deque<HWND> const& hwnds_, long const& width_, long const& height_)> ArrangeFunctionSTL;
      typedef void (* ArrangeFuncRef)(std::deque<HWND> const&, long const&, long const&);

      Layout(std::string, ArrangeFunctionSTL);
      std::string get_layout_name() const;
      void arrange(std::deque<HWND> const&);

    private:
      std::string m_layout_name;
      ArrangeFunctionSTL m_f;
  };
}

#endif


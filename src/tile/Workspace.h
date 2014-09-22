
#ifndef TILE_WORKSPACE_H
#define TILE_WORKSPACE_H

#include "../common_headers.h"

namespace Tile{
  class Workspace{
    private:
      std::string m_workspace_name;
      std::deque<HWND> m_managed_hwnds;
      void set_style(HWND const& hwnd_, std::vector<std::string> const& classnames_);

    public:
      Workspace(std::string const&);
      unsigned int size();
      HWND at(unsigned int const&);
      std::deque<HWND>::reverse_iterator const rbegin();
      std::deque<HWND>::reverse_iterator const rend();
      bool is_managed(HWND const&);
      void remanage_back(HWND const&);
      void remanage_front(HWND const&);
      void manage(HWND const&, std::vector<std::string> const&);
      void unmanage(HWND const&);
      std::deque<HWND> const& get_managed_hwnds() const;
      std::string get_workspace_name() const;
  };
}

#endif


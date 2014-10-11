
#ifndef TILE_WORKSPACE_H
#define TILE_WORKSPACE_H

#include "./common_headers.h"
#include "./Layout.h"
#include "./IgnoreClassNamesArranged.h"
#include "./NotApplyStyleToClassNames.h"

namespace Tile{
  class Workspace{
    private:
      std::string m_workspace_name;
      std::deque<HWND> m_managed_hwnds;
      std::shared_ptr<std::vector<Tile::Layout>> m_layouts;
      std::vector<Tile::Layout>::iterator m_layout_it;

      void set_style(HWND const&, Tile::NotApplyStyleToClassNames const&);

    public:
      Workspace(std::string const&, std::shared_ptr<std::vector<Tile::Layout>> const&);
      unsigned int count();
      HWND at(unsigned int const&);
      std::deque<HWND>::reverse_iterator const rbegin();
      std::deque<HWND>::reverse_iterator const rend();
      bool is_managed(HWND const&);
      void remanage_back(HWND const&);
      void remanage_front(HWND const&);
      void manage(HWND const&, Tile::NotApplyStyleToClassNames const&);
      void unmanage(HWND const&);
      std::deque<HWND> const& get_managed_hwnds() const;
      std::string get_workspace_name() const;
      void next_layout();
      boost::optional<std::string> get_layout_name() const;
      void arrange(Tile::IgnoreClassNamesArranged const&);
  };
}

#endif



#ifndef TILE_WORKSPACE_H
#define TILE_WORKSPACE_H

#include "../common_headers.h"

namespace Tile{
  class Workspace{
    private:
      std::string m_workspace_name;
      std::deque<HWND> m_managed_hwnds;
      void set_style(HWND hwnd_, std::vector<std::string> classnames_);

    public:
      Workspace(std::string workspace_name_);
      unsigned int size();
      HWND at(unsigned int i);
      std::deque<HWND>::iterator begin();
      std::deque<HWND>::iterator end();
      std::deque<HWND>::reverse_iterator rbegin();
      std::deque<HWND>::reverse_iterator rend();
      bool is_managed(HWND hwnd_);
      void remanage_back(HWND hwnd_);
      void remanage_front(HWND hwnd_);
      void manage(HWND hwnd_, std::vector<std::string> classnames_);
      void unmanage(HWND const hwnd_);
      std::deque<HWND> const& get_managed_hwnds() const;
      std::string get_workspace_name() const;
  };
}

#endif


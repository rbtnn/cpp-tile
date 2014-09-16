
#ifndef TILE_TILINGWINDOWMANAGER_H
#define TILE_TILINGWINDOWMANAGER_H

#include "../common_headers.h"
#include "./TilingWindowManager.h"
#include "./ConfigReader.h"
#include "./Key.h"
#include "./Workspace.h"
#include "./Layout.h"

namespace Tile{
  class TilingWindowManager{
    private:
      HINSTANCE m_hInstance;
      UINT m_shellhookid;
      HWND m_main_hwnd;
      std::string m_main_class_name;
      HWND m_statusline_hwnd;
      std::string m_statusline_class_name;
      HWND m_border_left_hwnd;
      HWND m_border_right_hwnd;
      HWND m_border_top_hwnd;
      HWND m_border_bottom_hwnd;
      std::string m_border_class_name;

      std::shared_ptr<ConfigReader> m_config;

      std::vector<std::shared_ptr<Key> > m_keys;

      std::vector<Tile::Workspace> m_workspaces;
      std::vector<Tile::Workspace>::iterator m_workspace_it;

      std::vector<Tile::Layout> m_layouts;
      std::vector<Tile::Layout>::iterator m_layout_it;


      void init_main();
      void init_statusline();
      void init_border();
      bool is_manageable(HWND const hwnd_);
      bool is_unmanageable(HWND const hwnd_);
      void regist_key(std::string key, void (Tile::TilingWindowManager::* f_)());
      void unmanage_all();
      void rescan();
      void run_shell();
      void exit_tile();
      void kill_client();
      void next_layout();
      void next_focus();
      void previous_focus();
      void move_to_workspace_1();
      void move_to_workspace_2();
      void move_to_workspace_3();
      void move_to_workspace_4();
      void move_to_workspace_5();
      void move_to_workspace_6();
      void move_to_workspace_7();
      void move_to_workspace_8();
      void move_to_workspace_9();
      void workspace_1();
      void workspace_2();
      void workspace_3();
      void workspace_4();
      void workspace_5();
      void workspace_6();
      void workspace_7();
      void workspace_8();
      void workspace_9();
      void focus_window_to_master();
      void move_to_workspace_of(unsigned int const i);
      void workspace_of(unsigned int const i);

    public:
      TilingWindowManager(HINSTANCE const&, std::string, std::vector<Tile::Layout>, std::shared_ptr<ConfigReader>);
      ~TilingWindowManager();
      UINT const start();
      void arrange();
      void manage(HWND hwnd_);
      void unmanage(HWND hwnd_);
      void call_key_method(UINT const& i_) const;
      void redraw_statusline();
      std::string get_layout_name();
      std::string get_workspace_name() const;
      long get_managed_window_size() const;
  };
}

#endif


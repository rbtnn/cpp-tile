
#ifndef TILE_TILINGWINDOWMANAGER_H
#define TILE_TILINGWINDOWMANAGER_H

#include "./common_headers.h"

#include "./ConfigReader.h"
#include "./Key.h"
#include "./Workspace.h"
#include "./Layout.h"
#include "./Recovery.h"

namespace Tile{
  class TilingWindowManager{
    private:
      NOTIFYICONDATA m_nid;
      HINSTANCE const m_hInstance;
      UINT m_shellhookid;
      HWND m_main_hwnd;
      std::string m_main_class_name;
      HWND m_border_left_hwnd;
      HWND m_border_right_hwnd;
      HWND m_border_top_hwnd;
      HWND m_border_bottom_hwnd;
      std::string m_border_class_name;

      std::shared_ptr<Tile::ConfigReader> const m_config;

      std::vector<std::shared_ptr<Tile::Key>> m_keys;

      std::vector<Tile::Workspace> m_workspaces;
      std::vector<Tile::Workspace>::iterator m_workspace_it;

      Tile::Recovery recovery;

      void init_main();
      void init_border();
      void regist_key(std::string const&, void (Tile::TilingWindowManager::*)());
      void unmanage_all();
      void rescan();
      void show_information();
      void toggle_border();
      void toggle_transparency_window();
      void run_process();
      void exit_tile();
      void kill_client();
      void next_layout();
      void next_focus();
      void previous_focus();
      template <unsigned int>
      void move_to_workspace();
      template <unsigned int>
      void workspace();
      void scrollwheel_up();
      void scrollwheel_down();
      void focus_window_to_master();
      void try_focus_managed_window();
      void swap_next();
      void swap_previous();

    public:
      TilingWindowManager(HINSTANCE const&, std::string const&, std::shared_ptr<std::vector<Tile::Layout>> const&, std::shared_ptr<ConfigReader> const&);
      ~TilingWindowManager();
      UINT const start();
      void arrange();
      void manage(HWND const& hwnd_);
      void unmanage(HWND const& hwnd_);
      void call_key_method(UINT const&);
  };
}

#endif


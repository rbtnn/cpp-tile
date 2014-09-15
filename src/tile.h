
#include "./common_headers.h"

namespace Tile{
  class ConfigReader{
    private:
      std::vector<std::string> m_ignore_classnames;
      std::vector<std::string> m_not_apply_style_to_classnames;
      std::map<std::string, std::string> m_keys;
      std::string m_inifile_path;
      std::vector<std::string> split(const std::string&, char);

    public:
      void reload();
      ConfigReader();
      std::map<std::string, std::string> const& get_keys() const;
      std::map<std::string, std::string> get_config_keys();
      std::string const& get_inipath() const;
      std::string get_inifile_path();
      std::vector<std::string> const& get_ignore_classnames() const;
      std::vector<std::string> const& get_not_apply_style_to_classnames() const;
      std::vector<std::string> get_config_settings_ignore_classnames();
      std::vector<std::string> get_config_settings_not_apply_style_to_classnames();
  };
  class Key{
    private:
      UINT const m_mod;
      UINT const m_k;
      UINT const m_hash_value;
      std::function<void(void)> const m_f;
      HWND const m_main_hwnd;

    public:
      Key(HWND const& hwnd_, UINT const& mod_, UINT const& k_, std::function<void(void)> const& f_);
      ~Key();
      UINT hash() const;
      void call() const;
  };
  class Layout{
    private:
      std::string m_layout_name;
      std::function<void(std::deque<HWND> const& hwnds_)> m_f;

    public:
      Layout(std::string, std::function<void(std::deque<HWND> const& hwnds_)>);
      std::string get_layout_name() const;
      void arrange(std::deque<HWND> const&);
  };
  class Workspace{
    private:
      std::string m_workspace_name;

      std::deque<HWND> m_managed_hwnds;
      std::map<HWND, RECT> m_managed_hwnd_to_rect;
      std::map<HWND, LONG> m_managed_hwnd_to_style;
      std::map<HWND, LONG> m_managed_hwnd_to_exstyle;

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
      void remanage(HWND hwnd_, std::vector<std::string> classnames_);
      void manage(HWND hwnd_, std::vector<std::string> classnames_);
      void unmanage(HWND const hwnd_);
      std::deque<HWND> const& get_managed_hwnds() const;
      std::string get_workspace_name() const;
  };
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
      bool is_manageable_focus(HWND const hwnd_);
      bool is_manageable_arrange(HWND const hwnd_);
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


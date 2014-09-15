
#ifndef TILE_CONFIGREADER_H
#define TILE_CONFIGREADER_H

#include "../common_headers.h"

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
}

#endif


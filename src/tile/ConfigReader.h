
#ifndef TILE_CONFIGREADER_H
#define TILE_CONFIGREADER_H

#include "../common_headers.h"

namespace Tile{
  class ConfigReader{
    private:
      std::vector<std::string> split(std::string const&, char);

    public:
      ConfigReader();
      std::string get_inifile_path();
      std::string get_run_process_path();
      std::vector<std::string> get_ignore_classnames();
      std::vector<std::string> get_not_apply_style_to_classnames();
      std::vector<std::string> get_layout_method_names();
      std::map<std::string, std::string> get_keys();
  };
}

#endif


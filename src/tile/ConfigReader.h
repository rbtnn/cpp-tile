
#ifndef TILE_CONFIGREADER_H
#define TILE_CONFIGREADER_H

#include "../common_headers.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace Tile{
  class ConfigReader{
    private:
      boost::property_tree::ptree m_pt;

    public:
      ConfigReader(char const * const);
      boost::optional<std::string> get_run_process_path() const;
      std::vector<std::string> get_ignore_classnames() const;
      std::vector<std::string> get_not_apply_style_to_classnames() const;
      std::vector<std::string> get_layout_method_names() const;
      std::map<std::string, std::string> get_keys() const;
  };
}

#endif


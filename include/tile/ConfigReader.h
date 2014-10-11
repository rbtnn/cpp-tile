
#ifndef TILE_CONFIGREADER_H
#define TILE_CONFIGREADER_H

#include "./common_headers.h"
#include "./NotApplyStyleToClassNames.h"
#include "./IgnoreClassNamesArranged.h"
#include "./LayoutMethodNames.h"
#include "./HotKey.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace Tile{
  class ConfigReader{
    private:
      boost::property_tree::ptree m_pt;

    public:
      ConfigReader(char const * const);
      boost::optional<std::string> get_run_process_path() const;
      Tile::IgnoreClassNamesArranged get_ignore_classnames_arranged() const;
      Tile::NotApplyStyleToClassNames get_not_apply_style_to_classnames() const;
      Tile::LayoutMethodNames get_layout_method_names() const;
      std::map<std::string, Tile::HotKey> get_keys() const;
  };
}

#endif


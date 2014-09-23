
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./ConfigReader.h"
#include "./NotApplyStyleToClassNames.h"
#include "./IgnoreClassNamesArranged.h"

namespace Tile{
  ConfigReader::ConfigReader(char const * const path_){
    boost::property_tree::read_json(path_, m_pt);
  }
  boost::optional<std::string> ConfigReader::get_run_process_path() const{
    return m_pt.get_optional<std::string>("settings.run_process_path");
  }
  Tile::IgnoreClassNamesArranged ConfigReader::get_ignore_classnames_arranged() const{
    Tile::IgnoreClassNamesArranged xs;
    auto const children = m_pt.get_child_optional("settings.ignore_classnames_arranged");
    if(children){
      for(auto const& x : *children){
        xs.value.push_back(x.second.get<std::string>(""));
      }
    }
    return xs;
  }
  Tile::NotApplyStyleToClassNames ConfigReader::get_not_apply_style_to_classnames() const{
    Tile::NotApplyStyleToClassNames xs;
    auto const children = m_pt.get_child_optional("settings.not_apply_style_to_classnames");
    if(children){
      for(auto const& x : *children){
        xs.value.push_back(x.second.get<std::string>(""));
      }
    }
    return xs;
  }
  Tile::LayoutMethodNames ConfigReader::get_layout_method_names() const{
    Tile::LayoutMethodNames xs;
    auto const children = m_pt.get_child_optional("settings.layout_method_names");
    if(children){
      for(auto const& x : *children){
        xs.value.push_back(x.second.get<std::string>(""));
      }
    }
    return xs;
  }
  std::map<std::string, std::string> ConfigReader::get_keys() const{
    std::map<std::string, std::string> m;
    auto const children = m_pt.get_child_optional("keys");
    if(children){
      for(auto const& x : *children){
        m.insert(std::map<std::string, std::string>::value_type(
              x.first,
              x.second.get<std::string>("")
              ));
      }
    }
    return m;
  }
}


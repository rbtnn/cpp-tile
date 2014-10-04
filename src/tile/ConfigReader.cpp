
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./ConfigReader.h"
#include "./NotApplyStyleToClassNames.h"
#include "./IgnoreClassNamesArranged.h"
#include "./HotKey.h"

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
  std::map<std::string, Tile::HotKey> ConfigReader::get_keys() const{
    std::map<std::string, Tile::HotKey> m;
    auto const children = m_pt.get_child_optional("keys");
    if(children){
      for(auto const& x : *children){
        auto const vk_str = x.second.get_optional<std::string>("vk");
        auto const vk_char = (vk_str ? (0 < vk_str->length() ? vk_str->at(0) : ' ' ) : ' ');
        auto const shift = x.second.get_optional<bool>("mod_shift");
        auto const control = x.second.get_optional<bool>("mod_control");
        auto const alt = x.second.get_optional<bool>("mod_alt");
        auto const win = x.second.get_optional<bool>("mod_win");
        m.insert(std::map<std::string, Tile::HotKey>::value_type(
              x.first,
              HotKey(vk_char,
                     (shift ? (*shift) : false),
                     (control ? (*control) : false),
                     (alt ? (*alt) : false),
                     (win ? (*win) : false))
              ));
      }
    }
    return m;
  }
}


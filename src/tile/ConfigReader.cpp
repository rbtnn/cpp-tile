
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./ConfigReader.h"

namespace Tile{
  ConfigReader::ConfigReader(char const * const path_){
    boost::property_tree::read_json(path_, m_pt);
  }
  boost::optional<std::string> ConfigReader::get_run_process_path() const{
    return m_pt.get_optional<std::string>("settings.run_process_path");
  }
  std::vector<std::string> ConfigReader::get_ignore_classnames() const{
    std::vector<std::string> xs;
    auto const children = m_pt.get_child_optional("settings.ignore_classnames");
    if(children){
      for(auto const& x : *children){
        xs.push_back(x.second.get<std::string>(""));
      }
    }
    return xs;
  }
  std::vector<std::string> ConfigReader::get_not_apply_style_to_classnames() const{
    std::vector<std::string> xs;
    auto const children = m_pt.get_child_optional("settings.not_apply_style_to_classnames");
    if(children){
      for(auto const& x : *children){
        xs.push_back(x.second.get<std::string>(""));
      }
    }
    return xs;
  }
  std::vector<std::string> ConfigReader::get_layout_method_names() const{
    std::vector<std::string> xs;
    auto const children = m_pt.get_child_optional("settings.layout_method_names");
    if(children){
      for(auto const& x : *children){
        xs.push_back(x.second.get<std::string>(""));
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


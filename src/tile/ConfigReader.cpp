
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./ConfigReader.h"

namespace Tile{
  std::vector<std::string> ConfigReader::split(const std::string& input_, char delimiter_){
    std::istringstream stream(input_);
    std::string field;
    std::vector<std::string> result;
    while (std::getline(stream, field, delimiter_)) {
      result.push_back(field);
    }
    return result;
  }
  std::vector<std::string> ConfigReader::get_config_settings_ignore_classnames(){
    char buffer[buffer_size];
    ::GetPrivateProfileString("settings", "IGNORE_CLASSNAMES", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
    return split(std::string(buffer), ',');
  }
  std::vector<std::string> ConfigReader::get_config_settings_not_apply_style_to_classnames(){
    char buffer[buffer_size];
    ::GetPrivateProfileString("settings", "NOT_APPLY_STYLE_TO_CLASSNAMES", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
    return split(std::string(buffer), ',');
  }
  std::map<std::string, std::string> ConfigReader::get_config_keys(){
    std::map<std::string, std::string> m;
    char buffer[buffer_size];
    ::GetPrivateProfileSection("keys", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
    std::string line = "";
    for(unsigned int i=0; i < (sizeof(buffer) / sizeof(char)) - 1; i++){
      if(buffer[i] == '\0'){
        auto xs = split(line, '=');
        line = "";
        if(1 < xs.size()){
          m.insert(std::map<std::string, std::string>::value_type(xs.at(0), xs.at(1)));
        }
        if(buffer[i + 1] == '\0'){
          break;
        }
      }
      else{
        line += buffer[i];
      }
    }
    return m;
  }
  std::string ConfigReader::get_inifile_path(){
    char szFileName[buffer_size];
    ::GetModuleFileName( NULL, szFileName, MAX_PATH);
    std::string str = std::string(szFileName);
    std::string::size_type pos = str.find(".exe", 0);
    if(pos != std::string::npos){
      str.replace(pos, 4, ".ini");
    }
    else{
      str = "";
    }
    return str;
  }
  ConfigReader::ConfigReader(){
    reload();
  }
  void ConfigReader::reload(){
    m_ignore_classnames = get_config_settings_ignore_classnames();
    m_not_apply_style_to_classnames = get_config_settings_not_apply_style_to_classnames();
    m_keys = get_config_keys();
    m_inifile_path = get_inifile_path();
  }
  std::vector<std::string> const& ConfigReader::get_ignore_classnames() const{
    return m_ignore_classnames;
  }
  std::vector<std::string> const& ConfigReader::get_not_apply_style_to_classnames() const{
    return m_not_apply_style_to_classnames;
  }
  std::map<std::string, std::string> const& ConfigReader::get_keys() const{
    return m_keys;
  }
  std::string const& ConfigReader::get_inipath() const{
    return m_inifile_path;
  }
}


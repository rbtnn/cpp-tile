
#include "../common_headers.h"
#include "../common_functions.h"
#include "../wndproc_functions.h"
#include "./ConfigReader.h"

namespace Tile{
  std::vector<std::string> ConfigReader::split(std::string const& input_, char delimiter_){
    std::istringstream stream(input_);
    std::string field;
    std::vector<std::string> result;
    while (std::getline(stream, field, delimiter_)) {
      result.push_back(field);
    }
    return result;
  }

  ConfigReader::ConfigReader(){
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
  std::string ConfigReader::get_run_process_path(){
    char buffer[buffer_size];
    ::GetPrivateProfileString("settings", "RUN_PROCESS_PATH", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
    return std::string(buffer);
  }
  std::vector<std::string> ConfigReader::get_ignore_classnames(){
    char buffer[buffer_size];
    ::GetPrivateProfileString("settings", "IGNORE_CLASSNAMES", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
    return split(std::string(buffer), ',');
  }
  std::vector<std::string> ConfigReader::get_not_apply_style_to_classnames(){
    char buffer[buffer_size];
    ::GetPrivateProfileString("settings", "NOT_APPLY_STYLE_TO_CLASSNAMES", "", buffer, sizeof(buffer) / sizeof(char), get_inifile_path().c_str());
    return split(std::string(buffer), ',');
  }
  std::map<std::string, std::string> ConfigReader::get_keys(){
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
}



#ifndef INIREADER_H
#define INIREADER_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class INIReader {
 public:
  INIReader(const std::string &filename) {
    m_error = false;
    std::ifstream file(filename);
    std::string section;

    if (!file) {
      m_error = true;
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      if (line.empty() || line[0] == ';' || line[0] == '#') {
        continue;
      }

      if (line[0] == '[') {
        size_t end = line.find(']');
        if (end != std::string::npos) {
          section = line.substr(1, end - 1);
        }
        continue;
      }

      size_t equalSign = line.find('=');
      if (equalSign != std::string::npos) {
        std::string key = line.substr(0, equalSign);
        std::string value = line.substr(equalSign + 1);
        m_data[section][key] = value;
      }
    }
  }

  std::string get(const std::string &section, const std::string &key,
                  const std::string &default_value = "") const {
    if (m_data.count(section) && m_data.at(section).count(key)) {
      return m_data.at(section).at(key);
    }
    return default_value;
  }

  int getInt(const std::string &section, const std::string &key,
             int default_value = 0) const {
    std::string value = get(section, key);
    if (!value.empty()) {
      return std::stoi(value);
    }
    return default_value;
  }

  bool parseError() const { return m_error; }

 private:
  std::map<std::string, std::map<std::string, std::string>> m_data;
  bool m_error;
};

#endif  // INIREADER_H

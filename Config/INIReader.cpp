#include "INIReader.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

INIReader &INIReader::getInstance() {
  static INIReader instance;
  return instance;
}

void INIReader::openFile(const QString &filename) {
  m_error = false;
  QFile file(filename);

  if (!file.open(QFile::ReadOnly)) {
    m_error = true;
    file.close();
    return;
  }
  QTextStream stream;
  stream.setEncoding(QStringConverter::Utf8);
  stream.setDevice(&file);

  QString section, key, value;

  while (!stream.atEnd()) {
    auto line = stream.readLine().toStdString();
    if (line.empty() || line[0] == ';' || line[0] == '#') {
      continue;
    }

    if (line[0] == '[') {
      size_t end = line.find(']');
      if (end != std::string::npos) {
        std::string sec = line.substr(1, end - 1);
        section = QString(sec.c_str());
      }
      continue;
    }

    size_t equalSign = line.find('=');

    if (equalSign != std::string::npos) {
      std::string k = line.substr(0, equalSign);
      key = QString(k.c_str());
      std::string v = line.substr(equalSign + 1);
      value = QString(v.c_str());
      m_data[section][key] = value;
    }
  }
}

QString INIReader::get(const QString &section, const QString &key,
                       const QString &value) const {
  if (m_data.count(section) && m_data[section].count(key))
    return m_data[section].value(key);
  return value;
}

bool INIReader::getBool(const QString &section, const QString &key,
                        bool value) const {
  if (m_data.count(section) && m_data[section].count(key)) {
    QString str = m_data[section].value(key).toLower();
    return str == "true" ? true : false;
  }
  return value;
}

int INIReader::getInt(const QString &section, const QString &key,
                      int value) const {
  if (m_data.count(section) && m_data[section].count(key))
    return m_data[section].value(key).toInt();
  return value;
}

float INIReader::getInt(const QString &section, const QString &key,
                        float value) const {
  if (m_data.count(section) && m_data[section].count(key))
    return m_data[section].value(key).toFloat();
  return value;
}

bool INIReader::parseError() const { return m_error; }

INIReader::INIReader() {}

INIReader::~INIReader() {}


#ifndef INIREADER_H
#define INIREADER_H

#include <QMap>

class INIReader {
 public:
  static INIReader &getInstance();

  void openFile(const QString &filename);

  QString get(const QString &section, const QString &key,
              const QString &value = "") const;

  bool getBool(const QString &section, const QString &key,
               bool value = false) const;

  int getInt(const QString &section, const QString &key, int value = 0) const;

  float getInt(const QString &section, const QString &key,
               float value = 0.0) const;

  bool parseError() const;

 private:
  INIReader();
  ~INIReader();
  INIReader(const INIReader &) = delete;
  INIReader &operator=(const INIReader &) = delete;

 private:
  QMap<QString, QMap<QString, QString>> m_data;
  bool m_error;
};

#endif  // INIREADER_H

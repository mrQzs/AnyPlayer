
#ifndef LOGGER_H
#define LOGGER_H

#include "INIReader.h"
#include "WriteLog.h"

enum class LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class WriteLog;

class Logger {
 public:
  static Logger &getInstance();
  void log(const QString &group, const QString &message, LogLevel level);
  void loadConfig(const QString &configFile);

 signals:

 private:
  Logger(LogLevel logLevel = LogLevel::DEBUG);
  ~Logger();
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

 private:
  QString formatTimestamp();
  QString levelToString(LogLevel level);
  LogLevel stringToLevel(const QString &levelStr);

 private:
  LogLevel m_logLevel;
  WriteLog &m_writeLog;
};

#endif  // LOGGER_H

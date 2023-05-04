#include "Logger.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <chrono>
#include <iomanip>
#include <stdexcept>

Logger &Logger::getInstance() {
  static Logger logger;
  return logger;
}

void Logger::log(const QString &group, const QString &message, LogLevel level) {
  if (level >= m_logLevel) {
    QString str =
        QString("[%1][%2][%3]%4\n")
            .arg(formatTimestamp(), levelToString(level), group, message);
    logQueue.push(str);
  }
}

void Logger::loadConfig(const QString &configFile) {
  INIReader &reader = INIReader::getInstance();
  reader.openFile(configFile);
  if (reader.parseError()) {
    return;
  }

  m_logLevel = stringToLevel(reader.get("logger", "log_level", "DEBUG"));
  fileMaxSize = reader.getInt("logger", "", 102400);
}

Logger::Logger(LogLevel logLevel)
    : m_logLevel(logLevel), m_writeLog(WriteLog::getInstance()) {
  m_writeLog.start();

  QString file = QDir::currentPath() + "\\config.ini";
  loadConfig(file);
}

Logger::~Logger() {
  m_writeLog.setStopFlag();
  m_writeLog.wait();
}

QString Logger::formatTimestamp() {
  return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

QString Logger::levelToString(LogLevel level) {
  switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARNING";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::CRITICAL:
      return "CRITICAL";
    default:
      return "UNKNOWN";
  }
}

LogLevel Logger::stringToLevel(const QString &levelStr) {
  if (levelStr == "DEBUG") {
    return LogLevel::DEBUG;
  } else if (levelStr == "INFO") {
    return LogLevel::INFO;
  } else if (levelStr == "WARNING") {
    return LogLevel::WARNING;
  } else if (levelStr == "ERROR") {
    return LogLevel::ERROR;
  } else if (levelStr == "CRITICAL") {
    return LogLevel::CRITICAL;
  } else {
    return LogLevel::DEBUG;
  }
}

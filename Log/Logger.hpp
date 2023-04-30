
#ifndef LOGGER_H
#define LOGGER_H

#include <QDir>
#include <QMetaType>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>

#include "INIReader.hpp"
#include "boost/lockfree/spsc_queue.hpp"

class Logger {
 public:
  enum class LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

  static Logger &getInstance() {
    static Logger logger;
    return logger;
  }

  void log(const std::string &group, const std::string &message,
           LogLevel level) {
    if (level >= m_logLevel) {
      std::stringstream ss;
      ss << "[" << formatTimestamp() << "] [" << levelToString(level) << "] ["
         << group << "] " << message;
      m_logQueue.push(ss.str());
    }
  }

  // 加载配置文件
  void loadConfig(const std::string &configFile) {
    INIReader reader(configFile);

    if (!reader.parseError()) {
      std::cerr << "Error: Can't load config file: " << configFile << std::endl;
      return;
    }

    m_logLevel = stringToLevel(reader.get("logger", "log_level", "DEBUG"));
    m_maxFileSize = static_cast<size_t>(
        reader.getInt("logger", "max_file_size", 10 * 1024 * 1024));
  }

 private:
  Logger(LogLevel logLevel = LogLevel::DEBUG,
         size_t maxFileSize = 10 * 1024 * 1024)
      : m_logLevel(logLevel),
        m_maxFileSize(maxFileSize),
        m_logQueue(1024),
        m_writeThread(&Logger::writeLogLoop, this) {
    qRegisterMetaType<LogLevel>("LogLevel");
  }

  ~Logger() {
    m_exitFlag.store(true);
    m_writeThread.join();
  }

  LogLevel &operator=(const LogLevel &) = delete;

 private:
  std::string formatTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);
    std::tm nowTm;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&nowTm, &nowTimeT);
#else
    localtime_r(&nowTimeT, &nowTm);
#endif

    std::stringstream ss;
    ss << std::put_time(&nowTm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
  }

  std::string levelToString(LogLevel level) {
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

  LogLevel stringToLevel(const std::string &levelStr) {
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

  void writeLogLoop() {
    while (!m_exitFlag.load()) {
      std::string message;
      if (m_logQueue.pop(message)) {
        try {
          writeFile(message);
        } catch (const std::runtime_error &e) {
          std::cerr << "Error writing log: " << e.what() << std::endl;
        }
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }

    // Flush any remaining messages in the queue
    std::string message;
    while (m_logQueue.pop(message)) {
      try {
        writeFile(message);
      } catch (const std::runtime_error &e) {
        std::cerr << "Error writing log: " << e.what() << std::endl;
      }
    }
  }

  void writeFile(const std::string &message) {
    // 获取当前日期
    std::time_t t = std::time(nullptr);
    char date[100];
    if (std::strftime(date, sizeof(date), "%Y-%m-%d", std::localtime(&t))) {
      // 按照日期生成日志文件名
      QString LogPath = QDir::currentPath() + "/Log/log-";

      std::string filename = LogPath.toStdString() + std::string(date) + ".txt";

      std::ofstream logFile(filename, std::ios::app);
      if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
      } else {
        std::cerr << "Error opening log file: " << filename << std::endl;
      }
    } else {
      std::cerr << "Error getting current date for log file name" << std::endl;
    }
  }

 private:
  LogLevel m_logLevel;
  size_t m_maxFileSize;
  boost::lockfree::spsc_queue<std::string> m_logQueue;
  std::thread m_writeThread;
  std::atomic<bool> m_exitFlag{false};
};

#endif  // LOGGER_H

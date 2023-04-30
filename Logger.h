
#ifndef LOGGER_H
#define LOGGER_H

// #include <chrono>
// #include <ctime>
// #include <fstream>
// #include <iomanip>
// #include <iostream>
// #include <mutex>
// #include <sstream>
// #include <stdexcept>
// #include <string>
// #include <thread>
// #include <unordered_map>

#include "boost/lockfree/spsc_queue.hpp"
//// #include "ini.h"

// class Logger {
//  public:
//   enum class LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

//  Logger(const std::string &filename, LogLevel logLevel = LogLevel::DEBUG,
//         size_t maxFileSize = 10 * 1024 * 1024)
//      : m_filename(filename),
//        m_logLevel(logLevel),
//        m_maxFileSize(maxFileSize),
//        m_logQueue(1024),
//        m_writeThread(&Logger::writeLogLoop, this) {
//    qRegisterMetaType<LogLevel>("LogLevel");
//  }

//  ~Logger() {
//    m_exitFlag.store(true);
//    m_writeThread.join();
//  }

//  void log(const std::string &group, const std::string &message,
//           LogLevel level) {
//    if (level >= m_logLevel) {
//      std::stringstream ss;
//      ss << "[" << formatTimestamp() << "] [" << levelToString(level) << "] ["
//         << group << "] " << message;
//      m_logQueue.push(ss.str());
//    }
//  }

//  static Logger &instance() {
//    static Logger logger("logfile.log", LogLevel::DEBUG);
//    return logger;
//  }

//  // 加载配置文件
//  void loadConfig(const std::string &configFile) {
//    INIReader reader(configFile);

//    if (reader.ParseError() < 0) {
//      std::cerr << "Error: Can't load config file: " << configFile <<
//      std::endl; return;
//    }

//    m_filename = reader.Get("logger", "filename", "logfile.log");
//    m_logLevel = stringToLevel(reader.Get("logger", "log_level", "DEBUG"));
//    m_maxFileSize = static_cast<size_t>(
//        reader.GetInteger("logger", "max_file_size", 10 * 1024 * 1024));
//  }

// private:
//  std::string m_filename;
//  LogLevel m_logLevel;
//  size_t m_maxFileSize;
//  boost::lockfree::spsc_queue<std::string> m_logQueue;
//  std::thread m_writeThread;
//  std::atomic<bool> m_exitFlag{false};

//  std::string formatTimestamp() {
//    auto now = std::chrono::system_clock::now();
//    auto now_time_t = std::chrono::system_clock::to_time_t(now);
//    std::tm tm_buf;
//    localtime_r(&now_time_t, &tm_buf);

//    std::stringstream ss;
//    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
//    return ss.str();
//  }

//  std::string levelToString(LogLevel level) {
//    switch (level) {
//      case LogLevel::DEBUG:
//        return "DEBUG";
//      case LogLevel::INFO:
//        return "INFO";
//      case LogLevel::WARNING:
//        return "WARNING";
//      case LogLevel::ERROR:
//        return "ERROR";
//      case LogLevel::CRITICAL:
//        return "CRITICAL";
//      default:
//        return "UNKNOWN";
//    }
//  }

//  LogLevel stringToLevel(const std::string &levelStr) {
//    if (levelStr == "DEBUG") {
//      return LogLevel::DEBUG;
//    } else if (levelStr == "INFO") {
//      return LogLevel::INFO;
//    } else if (levelStr == "WARNING") {
//      return LogLevel::WARNING;
//    } else if (levelStr == "ERROR") {
//      return LogLevel::ERROR;
//    } else if (levelStr == "CRITICAL") {
//      return LogLevel::CRITICAL;
//    } else {
//      return LogLevel::DEBUG;
//    }
//  }

//  void writeLogLoop() {
//    while (!m_exitFlag.load()) {
//      std::string message;
//      if (m_logQueue.pop(message)) {
//        try {
//          writeFile(message);
//        } catch (const std::runtime_error &e) {
//          std::cerr << "Error writing log: " << e.what() << std::endl;
//        }
//      } else {
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//      }
//    }

//    // Flush any remaining messages in the queue
//    std::string message;
//    while (m_logQueue.pop(message)) {
//      try {
//        writeFile(message);
//      } catch (const std::runtime_error &e) {
//        std::cerr << "Error writing log: " << e.what() << std::endl;
//      }
//    }
//  }

//  void writeFile(const std::string &message) {
//    std::ofstream file(m_filename, std::ios::app | std::ios::ate);
//    if (!file.is_open()) {
//      throw std::runtime_error("Failed to open log file");
//    }

//    file << message << std::endl;

//    if (file.tellp() >= static_cast<std::streamoff>(m_maxFileSize)) {
//      file.close();
//      std::string newFilename = m_filename + ".old";
//      std::rename(m_filename.c_str(), newFilename.c_str());
//    }
//  }
//};

#endif  // LOGGER_H

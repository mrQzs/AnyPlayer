
#include "UpdateFile.h"

#include <QAtomicInt>
#include <chrono>
#include <thread>

#include "GlobalVar.h"
#include "Logger.h"

constexpr size_t updateTime = 3600000;

UpdateFile &UpdateFile::getInstance() {
  static UpdateFile instance;
  return instance;
}

void UpdateFile::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void UpdateFile::run() {
  Logger::getInstance().log(UpdateFile::tr("线程"),
                            UpdateFile::tr("日志更新时间线程开始"),
                            LogLevel::INFO);

  using namespace std::chrono;

  auto start = high_resolution_clock::now();

  while (!m_stopFlag.loadAcquire()) {
    auto now = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(now - start).count();

    if (duration >= updateTime) {
      emit timeout();
      start = now;
    }

    std::this_thread::sleep_for(milliseconds(500));
  }

  Logger::getInstance().log(UpdateFile::tr("线程"),
                            UpdateFile::tr("日志更新时间线程结束"),
                            LogLevel::INFO);
}

UpdateFile::UpdateFile(QObject *parent) : QThread{parent} {}

UpdateFile::~UpdateFile() {}

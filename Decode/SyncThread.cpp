
#include "SyncThread.h"

#include <QAtomicInt>
#include <chrono>
#include <thread>

#include "Logger.h"

SyncThread &SyncThread::getInstance() {
  static SyncThread instance;
  return instance;
}

void SyncThread::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void SyncThread::run() {
  Logger::getInstance().log(SyncThread::tr("线程"),
                            SyncThread::tr("同步音视频线程启动"),
                            LogLevel::INFO);

  m_stopFlag.storeRelaxed(0);
  using namespace std::chrono;
  auto start = high_resolution_clock::now();

  while (!m_stopFlag.loadAcquire()) {
    auto now = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(now - start).count();

    if (duration >= m_time) {
      emit startSync();
      start = now;
    }

    // 休眠一段时间，以减少 CPU 占用
    std::this_thread::sleep_for(milliseconds(1));
  }

  Logger::getInstance().log(SyncThread::tr("线程"),
                            SyncThread::tr("同步音视频线程结束"),
                            LogLevel::INFO);
}

SyncThread::SyncThread(QObject *parent) : QThread{parent}, m_time{250} {}

SyncThread::~SyncThread() {}

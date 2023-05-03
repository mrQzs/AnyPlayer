
#include "SyncThread.h"

#include <chrono>
#include <thread>

#include "Logger.hpp"

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
  auto start = high_resolution_clock::now();  // 记录开始时间

  while (!m_stopFlag.loadAcquire()) {
    auto now = high_resolution_clock::now();  // 获取当前时间
    auto duration =
        duration_cast<milliseconds>(now - start).count();  // 计算经过的时间

    if (duration >= m_time) {  // 检查是否达到指定的计时器间隔（例如，1000毫秒）
      emit startSync();
      start = now;  // 更新开始时间
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

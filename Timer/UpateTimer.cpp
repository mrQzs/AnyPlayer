
#include "UpateTimer.h"

#include <QDebug>
#include <QTimer>
#include <chrono>
#include <thread>

#include "Logger.hpp"

UpateTimer &UpateTimer::getInstance() {
  static UpateTimer instance;
  return instance;
}

void UpateTimer::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void UpateTimer::setFreq(int freq) { m_freq = freq; }

int UpateTimer::getFreq() { return m_freq; }

void UpateTimer::run() {
  Logger::getInstance().log(UpateTimer::tr("线程"),
                            UpateTimer::tr("界面更新线程开始"), LogLevel::INFO);

  using namespace std::chrono;

  auto start = high_resolution_clock::now();  // 记录开始时间

  while (!m_stopFlag.loadAcquire()) {
    auto now = high_resolution_clock::now();  // 获取当前时间
    auto duration =
        duration_cast<milliseconds>(now - start).count();  // 计算经过的时间

    if (duration >= m_freq) {  // 检查是否达到指定的计时器间隔（例如，1000毫秒）
      emit timeout();
      start = now;  // 更新开始时间
    }

    // 休眠一段时间，以减少 CPU 占用
    std::this_thread::sleep_for(milliseconds(1));
  }

  Logger::getInstance().log(UpateTimer::tr("线程"),
                            UpateTimer::tr("界面更新线程结束"), LogLevel::INFO);
}

UpateTimer::UpateTimer() : m_timer(nullptr), m_stopFlag(0), m_freq(41) {}

UpateTimer::~UpateTimer() {}

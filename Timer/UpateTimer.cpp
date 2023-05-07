
#include "UpateTimer.h"

#include <chrono>
#include <thread>

#include "GlobalVar.h"
#include "Logger.h"

UpateTimer &UpateTimer::getInstance() {
  static UpateTimer instance;
  return instance;
}

void UpateTimer::run() {
  Logger::getInstance().log(UpateTimer::tr("线程"),
                            UpateTimer::tr("界面更新线程开始"), LogLevel::INFO);

  using namespace std::chrono;

  auto start = high_resolution_clock::now();

  while (!m_stopFlag.loadAcquire()) {
    auto now = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(now - start).count();

    if (duration >= g_freq.load()) {
      emit timeout();
      start = now;
    }

    std::this_thread::sleep_for(milliseconds(1));
  }

  Logger::getInstance().log(UpateTimer::tr("线程"),
                            UpateTimer::tr("界面更新线程结束"), LogLevel::INFO);
}

UpateTimer::UpateTimer() {}

UpateTimer::~UpateTimer() {}

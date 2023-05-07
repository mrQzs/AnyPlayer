
#include "Timer.h"

#include <chrono>
#include <thread>

#include "Log/Logger.h"

Timer::Timer(QObject *parent, const QString name, size_t time)
    : BaseThread{parent}, m_name{name}, m_time{time} {}

void Timer::setTime(size_t time) { m_time = time; }

void Timer::run() {
  Logger::getInstance().log(Timer::tr("线程"), m_name + Timer::tr("开始"),
                            LogLevel::INFO);

  using namespace std::chrono;

  auto start = high_resolution_clock::now();

  while (!m_stopFlag.loadAcquire()) {
    auto now = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(now - start).count();

    if (duration >= m_time) {
      emit timeout();
      start = now;
    }

    std::this_thread::sleep_for(milliseconds(500));
  }

  Logger::getInstance().log(Timer::tr("线程"), m_name + Timer::tr("结束"),
                            LogLevel::INFO);
}

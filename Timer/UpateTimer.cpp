
#include "UpateTimer.h"

#include <QDebug>
#include <QTimer>
#include <chrono>
#include <thread>

UpateTimer::UpateTimer(QObject *parent)
    : QThread(parent), m_timer(nullptr), m_stopFlag(0) {}

UpateTimer::~UpateTimer() {}

void UpateTimer::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void UpateTimer::run() {
  qDebug() << "界面更新线程开始";

  using namespace std::chrono;

  auto start = high_resolution_clock::now();  // 记录开始时间

  while (!m_stopFlag.loadAcquire()) {
    auto now = high_resolution_clock::now();  // 获取当前时间
    auto duration =
        duration_cast<milliseconds>(now - start).count();  // 计算经过的时间

    if (duration >= 25) {  // 检查是否达到指定的计时器间隔（例如，1000毫秒）
      emit timeout();
      start = now;  // 更新开始时间
    }

    // 休眠一段时间，以减少 CPU 占用
    std::this_thread::sleep_for(milliseconds(1));
  }

  qDebug() << "界面更新线程结束";
}

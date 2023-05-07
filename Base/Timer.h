
#ifndef TIMER_H
#define TIMER_H

#include "BaseThread.h"

class Timer : public BaseThread {
  Q_OBJECT
 public:
  explicit Timer(QObject *parent = nullptr, const QString name = "",
                 size_t time = 100);

 public:
  void setTime(size_t time);

 protected:
  void run();

 signals:
  void timeout();

 private:
  size_t m_time;
  QString m_name;
};

#endif  // TIMER_H

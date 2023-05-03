
#ifndef UPATETIMER_H
#define UPATETIMER_H

#include <QAtomicInt>
#include <QThread>

class QTimer;

class UpateTimer : public QThread {
  Q_OBJECT

 public:
  static UpateTimer &getInstance();
  void setStopFlag();

 protected:
  void run();

 signals:
  void timeout();

 private:
  UpateTimer();
  ~UpateTimer();
  UpateTimer(const UpateTimer &) = delete;
  UpateTimer &operator=(const UpateTimer &) = delete;

 private:
  QAtomicInt m_stopFlag;
};

#endif  // UPATETIMER_H

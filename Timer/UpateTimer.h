
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
  void setFreq(int freq);
  int getFreq();

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
  QTimer *m_timer;
  QAtomicInt m_stopFlag;
  int m_freq;
};

#endif  // UPATETIMER_H

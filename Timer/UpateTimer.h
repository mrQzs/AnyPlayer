
#ifndef UPATETIMER_H
#define UPATETIMER_H

#include <QAtomicInt>
#include <QThread>

class QTimer;

class UpateTimer : public QThread {
  Q_OBJECT
 public:
  explicit UpateTimer(QObject *parent = nullptr);
  ~UpateTimer();

 public:
  void setStopFlag();

 protected:
  void run();

 signals:
  void timeout();

 private:
  QTimer *m_timer;
  QAtomicInt m_stopFlag;
};

#endif  // UPATETIMER_H

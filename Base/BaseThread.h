
#ifndef BASETHREAD_H
#define BASETHREAD_H

#include <QAtomicInt>
#include <QThread>

class BaseThread : public QThread {
  Q_OBJECT
 public:
  explicit BaseThread(QObject *parent = nullptr);

 public:
  void setStopFlag();
  void setStartFlag();

 public:
  QAtomicInt m_stopFlag;

 signals:
};

#endif  // BASETHREAD_H

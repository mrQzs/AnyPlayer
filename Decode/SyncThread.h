
#ifndef SYNCTHREAD_H
#define SYNCTHREAD_H

#include <QThread>

class QAtomicInt;

class SyncThread : public QThread {
  Q_OBJECT
 public:
  static SyncThread& getInstance();
  void setStopFlag();

 protected:
  void run();

 signals:
  void startSync();

 private:
  explicit SyncThread(QObject* parent = nullptr);
  ~SyncThread();
  SyncThread(const SyncThread&) = delete;
  SyncThread& operator=(const SyncThread&) = delete;

 private:
  QAtomicInt m_stopFlag;
  int m_time;
};

#endif  // SYNCTHREAD_H

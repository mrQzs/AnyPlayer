
#ifndef WRITELOG_H
#define WRITELOG_H

#include <QThread>
#include <string>

#include "boost/lockfree/spsc_queue.hpp"

class QAtomicInt;
class UpdateFile;

extern QString currentTime;
extern QString fileName;
extern boost::lockfree::spsc_queue<QString> logQueue;
extern size_t fileMaxSize;

class WriteLog : public QThread {
  Q_OBJECT
 public:
  static WriteLog& getInstance();
  void setStopFlag();

 public slots:
  void updateFile();

 protected:
  void run();

 signals:

 private:
  void writeFile(const QString& message);
  QString getFile();

 private:
  explicit WriteLog(QObject* parent = nullptr);
  ~WriteLog();
  WriteLog(const WriteLog&) = delete;
  WriteLog& operator=(const WriteLog&) = delete;

 private:
  QAtomicInt m_stopFlog;
  UpdateFile& m_updateFile;
};

#endif  // WRITELOG_H

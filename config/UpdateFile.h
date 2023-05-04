
#ifndef UPDATEFILE_H
#define UPDATEFILE_H

#include <QThread>

class QAtomicInt;

class UpdateFile : public QThread {
  Q_OBJECT
 public:
  static UpdateFile &getInstance();
  void setStopFlag();

 protected:
  void run();

 signals:
  void timeout();

 private:
  explicit UpdateFile(QObject *parent = nullptr);
  ~UpdateFile();
  UpdateFile(const UpdateFile &) = delete;
  UpdateFile &operator=(const UpdateFile &) = delete;

 private:
  QAtomicInt m_stopFlag;
};

#endif  // UPDATEFILE_H

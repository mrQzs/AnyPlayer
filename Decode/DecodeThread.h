
#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <QAtomicInt>
#include <QObject>

class DecodeThread : public QObject {
  Q_OBJECT
 public:
  DecodeThread();
  ~DecodeThread();
  void startrun(const QString &file);
  void stop();
 signals:
  void getNewImage(QImage *image);

 private:
  QAtomicInt m_stopFlag;
};

#endif  // DECODETHREAD_H

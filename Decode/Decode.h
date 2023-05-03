
#ifndef DECODE_H
#define DECODE_H

#include <QAtomicInt>
#include <QThread>

class AudioDecode;
class VideoDecode;
class SyncThread;

class Decode : public QThread {
  Q_OBJECT
 public:
  Decode(QObject *parent = nullptr);
  ~Decode();

 public:
  void setStopFlag();
  void setSetFile(const QString &file);

 public slots:
  void slotSync();

 protected:
  void run();

 signals:
  void getNewImage(QImage *image);

 private:
  void decode();

 private:
  QAtomicInt m_stopFlag;
  QAtomicInt m_isDecode;
  QString m_file;

  AudioDecode &m_audioDecode;
  VideoDecode &m_videoDecode;
  SyncThread &m_syncThread;
};

#endif  // DECODE_H

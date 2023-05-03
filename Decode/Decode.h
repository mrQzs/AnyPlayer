
#ifndef DECODE_H
#define DECODE_H

#include <QAtomicInt>
#include <QThread>

class AudioDecode;
class VideoDecode;

class Decode : public QThread {
  Q_OBJECT
 public:
  Decode(QObject *parent = nullptr);
  ~Decode();

 public:
  void setStopFlag();
  void setSetFile(const QString &file);

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
};

#endif  // DECODE_H

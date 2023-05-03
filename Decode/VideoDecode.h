
#ifndef VIDEODECODE_H
#define VIDEODECODE_H

#include <QAtomicInt>
#include <QImage>
#include <QThread>

#include "GlobalVar.h"

class QImage;

class VideoDecode : public QThread {
  Q_OBJECT

 public:
  static VideoDecode &getInstance();
  void setStopFlag();
  void setCodecContext(AVCodecContext *videoCodecContex);

 protected:
  void run();

 signals:

 private:
  VideoDecode(QObject *parent = nullptr);
  ~VideoDecode();
  VideoDecode(const VideoDecode &) = delete;
  VideoDecode &operator=(const VideoDecode &) = delete;

 private:
  void decode();

 private:
  QAtomicInt m_stopFlag;
  QString m_file;
  AVCodecContext *m_videoCodecContex;
};

#endif  // VIDEODECODE_H

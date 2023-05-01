
#ifndef VIDEODECODE_H
#define VIDEODECODE_H

#include <ffmpegIn.h>

#include <QAtomicInt>
#include <QImage>
#include <QThread>

class QImage;

class VideoDecode : public QThread {
  Q_OBJECT

 public:
  static VideoDecode &getInstance();
  void setStopFlag();
  void setSetFile(const QString &file);

  // QThread interface
 protected:
  void run();

 signals:
  void getNewImage(QImage *image);

 private:
  VideoDecode(QObject *parent = nullptr);
  ~VideoDecode();
  VideoDecode(const VideoDecode &) = delete;
  VideoDecode &operator=(const VideoDecode &) = delete;

 private:
  void decode();
  QImage avFrameToQImage(AVFrame *frame);

 private:
  QAtomicInt m_stopFlag;
  QAtomicInt m_isDecode;
  QString m_file;
  // QList<QImage> m_img;
};

#endif  // VIDEODECODE_H

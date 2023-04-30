
#ifndef VIDEOPLAYWIDGET_H
#define VIDEOPLAYWIDGET_H

#include <QPainter>
#include <QQueue>
#include <QTimer>
#include <QWidget>

class UpateTimer;
class QThread;

class VideoPlayWidget : public QWidget {
  Q_OBJECT
 public:
  explicit VideoPlayWidget(QWidget *parent = nullptr);
  ~VideoPlayWidget();

 public:
  void setImage(QImage *image);

 public slots:
  void drawImg();

 protected:
  void paintEvent(QPaintEvent *) override;

 private:
  QQueue<QImage *> m_queue;
  QImage m_image;
  UpateTimer *m_timer;
};

#endif  // VIDEOPLAYWIDGET_H

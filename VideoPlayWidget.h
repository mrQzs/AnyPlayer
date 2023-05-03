
#ifndef VIDEOPLAYWIDGET_H
#define VIDEOPLAYWIDGET_H

#include <QPainter>
#include <QTimer>
#include <QWidget>

class UpateTimer;
class QThread;

class VideoPlayWidget : public QWidget {
  Q_OBJECT
 public:
  explicit VideoPlayWidget(QWidget *parent = nullptr);
  ~VideoPlayWidget();

 protected:
  void paintEvent(QPaintEvent *) override;

 private:
  QImage m_image;
  UpateTimer &m_timer;
};

#endif  // VIDEOPLAYWIDGET_H

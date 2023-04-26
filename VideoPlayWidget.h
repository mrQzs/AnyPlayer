
#ifndef VIDEOPLAYWIDGET_H
#define VIDEOPLAYWIDGET_H

#include <QPainter>
#include <QWidget>

class VideoPlayWidget : public QWidget {
  Q_OBJECT
 public:
  explicit VideoPlayWidget(QWidget *parent = nullptr);

 public:
  void setImage(const QImage &image);

 protected:
  void paintEvent(QPaintEvent *) override;

 private:
  QImage m_image;
};

#endif  // VIDEOPLAYWIDGET_H

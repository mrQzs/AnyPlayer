
#include "VideoPlayWidget.h"

VideoPlayWidget::VideoPlayWidget(QWidget *parent) : QWidget{parent} {}

void VideoPlayWidget::setImage(const QImage &image) {
  m_image = image;
  update();
}

void VideoPlayWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  if (!m_image.isNull()) {
    // 将图像绘制到窗口上，根据需要进行缩放
    painter.drawImage(rect(), m_image, m_image.rect());
  }
}


#include "VideoPlayWidget.h"

#include <QThread>

#include "UpateTimer.h"

VideoPlayWidget::VideoPlayWidget(QWidget *parent)
    : QWidget{parent}, m_timer(new UpateTimer(this)) {
  connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_timer->start();
}

VideoPlayWidget::~VideoPlayWidget() {
  m_timer->setStopFlag();
  m_timer->wait();
  delete m_timer;
}

void VideoPlayWidget::setImage(QImage *image) { m_queue.enqueue(image); }

void VideoPlayWidget::drawImg() {}

void VideoPlayWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  if (!m_queue.isEmpty()) {
    QImage *img = m_queue.dequeue();
    painter.drawImage(rect(), *img, img->rect());
    delete img;
  }
}

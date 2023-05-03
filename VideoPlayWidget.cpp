
#include "VideoPlayWidget.h"

#include <GlobalVar.h>

#include <QDebug>
#include <QThread>

#include "UpateTimer.h"

VideoPlayWidget::VideoPlayWidget(QWidget *parent)
    : QWidget{parent}, m_timer(UpateTimer::getInstance()) {
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_timer.start();
}

VideoPlayWidget::~VideoPlayWidget() {
  m_timer.setStopFlag();
  m_timer.wait();
}

void VideoPlayWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  QImage *img = nullptr;
  if (imageQueue.pop(img)) {
    if (img) painter.drawImage(rect(), *img, img->rect());
    delete img;
  }
}

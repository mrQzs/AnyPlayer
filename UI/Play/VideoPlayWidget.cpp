
#include "VideoPlayWidget.h"

#include <QDebug>
#include <QThread>

#include "GlobalVar.h"
#include "Logger.h"
#include "UpateTimer.h"

VideoPlayWidget::VideoPlayWidget(QWidget *parent)
    : QWidget{parent}, m_timer(UpateTimer::getInstance()) {
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_timer.start();
}

VideoPlayWidget::~VideoPlayWidget() {
  m_timer.setStopFlag();
  m_timer.wait();

  // 释放资源
  av_frame_free(&rgbFrame);
  sws_freeContext(swsCtx);
}

void VideoPlayWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  if (!videoFrameQueue.empty()) {
    widgetW = width();
    widgetH = height();
    AVFrame *frame{0};
    videoFrameQueue.pop(frame);
    if (frame) {
      videoPts = frame->pts * av_q2d(g_videoStream->time_base);
      QImage img = avFrameToQImage(frame);
      painter.drawImage(0, 0, img);
      video_frame_pool.release(frame);
    }
  }
}


#include "VideoPlayWidget.h"

#include <QDebug>
#include <QThread>

#include "GlobalVar.h"
#include "Logger.hpp"
#include "UpateTimer.h"

QImage avFrameToQImage(AVFrame *frame);

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
  if (!videoFrameQueue.empty()) {
    AVFrame *frame = nullptr;
    videoFrameQueue.pop(frame);
    if (frame) {
      videoPts = frame->pts * av_q2d(g_videoStream->time_base);
      QImage img = avFrameToQImage(frame);
      painter.drawImage(rect(), img, img.rect());
      av_frame_free(&frame);
    }
  }
}

QImage avFrameToQImage(AVFrame *frame) {
  // 获取 AVFrame 的宽度和高度
  int width = frame->width;
  int height = frame->height;

  // 创建一个 SwsContext，用于将 AVFrame 的像素格式转换为 QImage 支持的格式
  SwsContext *swsCtx =
      sws_getContext(width, height, (AVPixelFormat)frame->format, width, height,
                     AV_PIX_FMT_RGB32, SWS_BILINEAR, NULL, NULL, NULL);

  if (!swsCtx) {
    // 如果无法创建 SwsContext，请返回一个空 QImage
    return QImage();
  }

  // 分配一个新的 AVFrame 用于存储转换后的像素数据
  AVFrame *rgbFrame = av_frame_alloc();
  rgbFrame->width = width;
  rgbFrame->height = height;
  rgbFrame->format = AV_PIX_FMT_RGB32;

  // 为 rgbFrame 分配缓冲区
  int ret = av_frame_get_buffer(rgbFrame, 1);
  if (ret < 0) {
    // 如果无法为 AVFrame 分配缓冲区，请返回一个空 QImage
    av_frame_free(&rgbFrame);
    sws_freeContext(swsCtx);
    return QImage();
  }

  // 使用 sws_scale 将原始 AVFrame 的像素格式转换为 AV_PIX_FMT_RGB32
  sws_scale(swsCtx, frame->data, frame->linesize, 0, height, rgbFrame->data,
            rgbFrame->linesize);

  // 使用转换后的像素数据创建一个 QImage
  QImage image(rgbFrame->data[0], width, height, rgbFrame->linesize[0],
               QImage::Format_RGB32);

  // 复制 QImage，以便我们可以安全地释放 AVFrame
  QImage result = image.copy();

  // 释放资源
  av_frame_free(&rgbFrame);
  sws_freeContext(swsCtx);

  return result;
}


#include "VideoDecode.h"

#include "GlobalVar.h"
#include "Logger.hpp"

QImage *avFrameToQImage(AVFrame *frame);

VideoDecode &VideoDecode::getInstance() {
  static VideoDecode instance;
  return instance;
}

void VideoDecode::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void VideoDecode::setCodecContext(AVCodecContext *videoCodecContex) {
  m_videoCodecContex = videoCodecContex;
}

void VideoDecode::run() {
  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程启动"),
                            LogLevel::INFO);

  AVPacket packet;
  while (!m_stopFlag.loadAcquire()) {
    if (videoPacketQueue.pop(packet)) {
      AVFrame *frame = av_frame_alloc();
      avcodec_send_packet(m_videoCodecContex, &packet);
      if (avcodec_receive_frame(m_videoCodecContex, frame) == 0) {
        //        QImage *img = avFrameToQImage(frame);
        //        imageQueue.push(img);
      }
      av_frame_free(&frame);
      av_packet_unref(&packet);
    } else
      std::this_thread::yield();
  }

  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程释放剩余的队列"),
                            LogLevel::INFO);

  while (videoPacketQueue.pop(packet)) {
    av_packet_unref(&packet);
  }

  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程结束"),
                            LogLevel::INFO);
}

VideoDecode::VideoDecode(QObject *parent) : QThread(parent), m_stopFlag(0) {}

VideoDecode::~VideoDecode() {}

void VideoDecode::decode() {}

QImage *avFrameToQImage(AVFrame *frame) {
  // 获取 AVFrame 的宽度和高度
  int width = frame->width;
  int height = frame->height;

  // 创建一个 SwsContext，用于将 AVFrame 的像素格式转换为 QImage 支持的格式
  SwsContext *swsCtx =
      sws_getContext(width, height, (AVPixelFormat)frame->format, width, height,
                     AV_PIX_FMT_RGB32, SWS_BILINEAR, NULL, NULL, NULL);

  if (!swsCtx) {
    // 如果无法创建 SwsContext，请返回一个空 QImage
    return nullptr;
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
    return nullptr;
  }

  // 使用 sws_scale 将原始 AVFrame 的像素格式转换为 AV_PIX_FMT_RGB32
  sws_scale(swsCtx, frame->data, frame->linesize, 0, height, rgbFrame->data,
            rgbFrame->linesize);

  // 使用转换后的像素数据创建一个 QImage
  QImage image(rgbFrame->data[0], width, height, rgbFrame->linesize[0],
               QImage::Format_RGB32);

  // 复制 QImage，以便我们可以安全地释放 AVFrame
  QImage *result = new QImage(image.copy());

  // 释放资源
  av_frame_free(&rgbFrame);
  sws_freeContext(swsCtx);

  return result;
}


#include "VideoDecode.h"

#include "GlobalVar.h"
#include "Logger.hpp"

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
  m_stopFlag.storeRelaxed(0);
  AVPacket packet;
  while (!m_stopFlag.loadAcquire()) {
    if (!videoPacketQueue.empty()) {
      videoPacketQueue.pop(packet);
      AVFrame *frame = av_frame_alloc();
      avcodec_send_packet(m_videoCodecContex, &packet);
      if (avcodec_receive_frame(m_videoCodecContex, frame) == 0) {
        videoFrameQueue.push(frame);
      } else {
        av_frame_free(&frame);
      }
      av_packet_unref(&packet);
    } else
      QThread::usleep(1);
  }

  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程释放剩余的队列"),
                            LogLevel::INFO);

  while (!videoFrameQueue.empty()) {
    AVFrame *frame = nullptr;
    videoFrameQueue.pop(frame);
    if (frame) {
      av_frame_unref(frame);
      av_frame_free(&frame);
    }
  }

  while (!videoPacketQueue.empty()) {
    videoPacketQueue.pop(packet);
    av_packet_unref(&packet);
  }

  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程结束"),
                            LogLevel::INFO);
}

VideoDecode::VideoDecode(QObject *parent) : QThread(parent), m_stopFlag(0) {}

VideoDecode::~VideoDecode() {}

void VideoDecode::decode() {}


#include "VideoDecode.h"

#include <QAtomicInt>
#include <QDebug>

#include "GlobalVar.h"
#include "Logger.h"

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
  AVPacket *packet{0};
  AVFrame *frame{0};
  while (!m_stopFlag.loadAcquire()) {
    if (!videoPacketQueue.empty()) {
      videoPacketQueue.pop(packet);
      frame = video_frame_pool.allocate();
      avcodec_send_packet(m_videoCodecContex, packet);

      if (avcodec_receive_frame(m_videoCodecContex, frame) == 0) {
        videoFrameQueue.push(frame);
      } else
        video_frame_pool.release(frame);

      packet_pool.release_packet(packet);
    } else
      QThread::usleep(10);
  }

  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程释放剩余的队列"),
                            LogLevel::INFO);

  while (!videoFrameQueue.empty()) {
    videoFrameQueue.pop(frame);
    if (frame) video_frame_pool.release(frame);
  }

  while (!videoPacketQueue.empty()) {
    videoPacketQueue.pop(packet);
    packet_pool.release_packet(packet);
  }

  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程结束"),
                            LogLevel::INFO);
}

VideoDecode::VideoDecode(QObject *parent) : QThread(parent), m_stopFlag(0) {}

VideoDecode::~VideoDecode() {}

void VideoDecode::decode() {}


#include "VideoDecode.h"

#include <QDebug>
#include <QImage>
#include <QThread>

VideoDecode::VideoDecode(QObject *parent)
    : QThread(parent), m_stopFlag(0), m_isDecode(0) {}

VideoDecode::~VideoDecode() {}

void VideoDecode::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void VideoDecode::setSetFile(const QString &file) {
  m_isDecode.storeRelaxed(1);
  m_file = file;
}

void VideoDecode::run() {
  qDebug() << "视频解码线程启动";
  while (!m_stopFlag.loadAcquire()) {
    if (m_isDecode.loadAcquire()) {
      m_isDecode.storeRelaxed(0);
      decode();
    }
  }
  qDebug() << "视频解码线程停止";
}

void VideoDecode::decode() {
  AVFormatContext *formatContext = nullptr;
  if (avformat_open_input(&formatContext, m_file.toLocal8Bit().constData(),
                          nullptr, nullptr) != 0) {
    qDebug() << "创建AVFormatContext失败";
    return;
  } else
    qDebug() << "创建AVFormatContext成功";

  if (avformat_find_stream_info(formatContext, nullptr) < 0) {
    qDebug() << "检索流信息失败";
    return;
  } else
    qDebug() << "检索流信息成功";

  int videoStreamIndex = -1;
  for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
    if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      videoStreamIndex = i;
      break;
    }
  }

  if (videoStreamIndex == -1) {
    qDebug() << "未找到视频流";
    return;
  } else
    qDebug() << "找到视频流";

  AVCodecParameters *codecParameters =
      formatContext->streams[videoStreamIndex]->codecpar;
  const AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);

  if (!codec) {
    qDebug() << "未找到编解码器";
    return;
  } else
    qDebug() << "找到编解码器";

  AVCodecContext *codecContext = avcodec_alloc_context3(codec);
  avcodec_parameters_to_context(codecContext, codecParameters);

  if (avcodec_open2(codecContext, codec, nullptr) < 0) {
    qDebug() << "创建AVCodecContext失败";
    return;
  } else
    qDebug() << "创建AVCodecContext成功";

  AVFrame *frame = av_frame_alloc();
  AVPacket packet;

  while (av_read_frame(formatContext, &packet) >= 0) {
    if (packet.stream_index == videoStreamIndex) {
      // 发送包到解码器
      if (avcodec_send_packet(codecContext, &packet) != 0) {
        // 错误处理
        qDebug() << "发送包到解码器失败";
      } else {
        while (avcodec_receive_frame(codecContext, frame) == 0) {
          auto image = new QImage(avFrameToQImage(frame));
          emit getNewImage(image);
          if (m_stopFlag.loadAcquire()) break;
        }
      }
    }
    av_packet_unref(&packet);
    if (m_stopFlag.loadAcquire()) break;
  }

  av_frame_free(&frame);
  avcodec_free_context(&codecContext);
  avformat_close_input(&formatContext);
}

QImage VideoDecode::avFrameToQImage(AVFrame *frame) {
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

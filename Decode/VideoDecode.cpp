
#include "VideoDecode.h"

#include <SDL.h>

#include <QDebug>
#include <QImage>
#include <QMutex>
#include <QThread>

#include "Logger.hpp"
#include "UpateTimer.h"

QMutex audioQueueMutex;
std::queue<AVFrame *> audioQueue;

void audio_callback(void *userdata, Uint8 *stream, int len) {
  AVCodecContext *audioCodecContext = static_cast<AVCodecContext *>(userdata);
  int bufferSize = 0;
  uint8_t *buffer = nullptr;

  while (len > 0) {
    if (bufferSize == 0) {
      AVFrame *frame = nullptr;
      {
        QMutexLocker locker(&audioQueueMutex);
        if (audioQueue.empty()) {
          return;
        }
        frame = audioQueue.front();
        audioQueue.pop();
      }

      if (frame) {
        int numSamples = av_get_bytes_per_sample(audioCodecContext->sample_fmt);
        bufferSize =
            frame->nb_samples * numSamples * audioCodecContext->channels;
        buffer = frame->data[0];
      }
    }

    int bytesToCopy = std::min(len, bufferSize);
    memcpy(stream, buffer, bytesToCopy);
    stream += bytesToCopy;
    buffer += bytesToCopy;
    bufferSize -= bytesToCopy;
    len -= bytesToCopy;
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

VideoDecode &VideoDecode::getInstance() {
  static VideoDecode instance;
  return instance;
}

void VideoDecode::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void VideoDecode::setSetFile(const QString &file) {
  m_isDecode.storeRelaxed(1);
  m_file = file;
}

void VideoDecode::run() {
  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程启动"),
                            LogLevel::INFO);

  while (!m_stopFlag.loadAcquire()) {
    if (m_isDecode.loadAcquire()) {
      m_isDecode.storeRelaxed(0);
      decode();
    }
  }

  Logger::getInstance().log(VideoDecode::tr("线程"),
                            VideoDecode::tr("视频解码线程结束"),
                            LogLevel::INFO);
}

VideoDecode::VideoDecode(QObject *parent)
    : QThread(parent), m_stopFlag(0), m_isDecode(0) {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    throw std::runtime_error("Failed to initialize SDL: " +
                             std::string(SDL_GetError()));
  }
}

VideoDecode::~VideoDecode() {}

void VideoDecode::decode() {
  AVFormatContext *formatContext = nullptr;
  if (avformat_open_input(&formatContext, m_file.toLocal8Bit().constData(),
                          nullptr, nullptr) != 0) {
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("创建AVFormatContext失败"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("创建AVFormatContext成功"),
                              LogLevel::INFO);

  if (avformat_find_stream_info(formatContext, nullptr) < 0) {
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("检索流信息失败"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("检索流信息成功"),
                              LogLevel::INFO);

  int videoStreamIndex = -1;
  for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
    if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      videoStreamIndex = i;
      break;
    }
  }

  if (videoStreamIndex == -1) {
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("未找到视频流"), LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("找到视频流"), LogLevel::INFO);

  int audioStreamIndex = -1;
  for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
    if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      audioStreamIndex = i;
      break;
    }
  }

  if (audioStreamIndex == -1) {
    qDebug() << "No audio stream found";
    return;
  }

  AVCodecParameters *vCodecParameters =
      formatContext->streams[videoStreamIndex]->codecpar;
  AVCodecParameters *aCodecParameters =
      formatContext->streams[audioStreamIndex]->codecpar;

  const AVCodec *videoCodec = avcodec_find_decoder(vCodecParameters->codec_id);
  const AVCodec *audioCodec = avcodec_find_decoder(aCodecParameters->codec_id);

  if (!videoCodec) {
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("未找到编解码器"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("找到编解码器"), LogLevel::INFO);

  if (!audioCodec) {
    Logger::getInstance().log(VideoDecode::tr("音频解码"),
                              VideoDecode::tr("未找到编解码器"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(VideoDecode::tr("音频解码"),
                              VideoDecode::tr("找到编解码器"), LogLevel::INFO);

  AVCodecContext *videoCodecContext = avcodec_alloc_context3(videoCodec);
  AVCodecContext *audioCodecContext = avcodec_alloc_context3(audioCodec);

  avcodec_parameters_to_context(videoCodecContext, vCodecParameters);
  avcodec_parameters_to_context(audioCodecContext, aCodecParameters);

  if (avcodec_open2(videoCodecContext, videoCodec, nullptr) < 0) {
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("创建AVCodecContext失败"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(VideoDecode::tr("视频解码"),
                              VideoDecode::tr("创建AVCodecContext成功"),
                              LogLevel::INFO);

  if (avcodec_open2(audioCodecContext, audioCodec, nullptr) < 0) {
    Logger::getInstance().log(VideoDecode::tr("音频解码"),
                              VideoDecode::tr("创建AVCodecContext失败"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(VideoDecode::tr("音频解码"),
                              VideoDecode::tr("创建AVCodecContext成功"),
                              LogLevel::INFO);

  AVStream **stream = formatContext->streams;
  int den = (*stream)->avg_frame_rate.den;
  int num = (*stream)->avg_frame_rate.num;
  float rate = (float)num / (float)den;
  float freq = 1000.0 / rate;

  UpateTimer::getInstance().setFreq(freq);

  AVFrame *frame = av_frame_alloc();
  AVPacket packet;

  SDL_Init(SDL_INIT_AUDIO);

  SDL_AudioSpec audioSpec;
  audioSpec.freq = audioCodecContext->sample_rate;
  audioSpec.format = AUDIO_S16SYS;
  audioSpec.channels = audioCodecContext->channels;
  audioSpec.samples = 1024;
  audioSpec.callback = audio_callback;
  audioSpec.userdata = audioCodecContext;

  SwrContext *swrCtx = swr_alloc();

  av_opt_set_int(swrCtx, "in_channel_layout",
                 av_get_default_channel_layout(audioCodecContext->channels), 0);
  av_opt_set_int(swrCtx, "out_channel_layout",
                 av_get_default_channel_layout(audioSpec.channels), 0);
  av_opt_set_int(swrCtx, "in_sample_rate", audioCodecContext->sample_rate, 0);
  av_opt_set_int(swrCtx, "out_sample_rate", audioSpec.freq, 0);
  av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", audioCodecContext->sample_fmt,
                        0);
  av_opt_set_sample_fmt(swrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

  if (swr_init(swrCtx) < 0) {
    return;
  }

  if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
    return;
  }

  SDL_PauseAudio(0);

  while (av_read_frame(formatContext, &packet) >= 0) {
    if (packet.stream_index == videoStreamIndex) {
      avcodec_send_packet(videoCodecContext, &packet);
      if (avcodec_receive_frame(videoCodecContext, frame) == 0) {
        auto image = new QImage(avFrameToQImage(frame));
        emit getNewImage(image);
        if (m_stopFlag.loadAcquire()) break;
      }
    } else if (packet.stream_index == audioStreamIndex) {
      AVFrame *aframe = av_frame_alloc();
      avcodec_send_packet(audioCodecContext, &packet);
      if (avcodec_receive_frame(audioCodecContext, aframe) == 0) {
        AVFrame *resampledFrame = av_frame_alloc();
        resampledFrame->channel_layout =
            av_get_default_channel_layout(audioSpec.channels);
        resampledFrame->format = AV_SAMPLE_FMT_S16;
        resampledFrame->sample_rate = audioSpec.freq;
        resampledFrame->channels = audioSpec.channels;

        if (swr_convert_frame(swrCtx, resampledFrame, aframe) < 0) {
          fprintf(stderr, "swr_convert_frame() failed\n");
          av_frame_free(&resampledFrame);
        } else {
          audioQueueMutex.lock();
          audioQueue.push(resampledFrame);
          audioQueueMutex.unlock();
        }
      } else {
        av_frame_free(&aframe);
      }
    } else {
      av_packet_unref(&packet);
    }
    if (m_stopFlag.loadAcquire()) break;
  }

  SDL_CloseAudio();
  SDL_Quit();

  av_frame_free(&frame);

  avcodec_close(videoCodecContext);
  avcodec_close(audioCodecContext);

  avcodec_free_context(&videoCodecContext);
  avcodec_free_context(&audioCodecContext);

  avformat_close_input(&formatContext);
  avformat_free_context(formatContext);
}

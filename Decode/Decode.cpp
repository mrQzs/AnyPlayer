
#include "Decode.h"

#include <QDebug>
#include <QImage>
#include <QMutex>
#include <QThread>
#include <chrono>

#include "AudioDecode.h"
#include "GlobalVar.h"
#include "Logger.h"
#include "Timer.h"
#include "VideoDecode.h"

Decode::Decode(QObject *parent)
    : QThread(parent),
      m_stopFlag(0),
      m_isDecode(0),
      m_audioDecode(AudioDecode::getInstance()),
      m_videoDecode(VideoDecode::getInstance()),
      m_syncThread{new Timer(this, Decode::tr("同步音视频线程"), 250)} {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    throw std::runtime_error("Failed to initialize SDL: " +
                             std::string(SDL_GetError()));
  }

  connect(m_syncThread, SIGNAL(timeout()), this, SLOT(slotSync()));
}

Decode::~Decode() {}

void Decode::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void Decode::setSetFile(const QString &file) {
  m_isDecode.storeRelaxed(1);
  m_file = file;
}

void Decode::slotSync() {
  double diff = audioPts - videoPts;

  if (abs(diff) > 0.05) {
    if (diff > 0)
      g_freq--;
    else
      g_freq++;
  }
}

void Decode::run() {
  Logger::getInstance().log(Decode::tr("线程"), Decode::tr("解码线程启动"),
                            LogLevel::INFO);
  while (!m_stopFlag.loadAcquire()) {
    if (m_isDecode.loadAcquire()) {
      m_isDecode.storeRelaxed(0);
      decode();
    } else
      QThread::usleep(500);
  }

  Logger::getInstance().log(Decode::tr("线程"), Decode::tr("解码线程结束"),
                            LogLevel::INFO);
}

void Decode::decode() {
  AVFormatContext *formatContext = nullptr;
  if (avformat_open_input(&formatContext, m_file.toLocal8Bit().constData(),
                          nullptr, nullptr) != 0) {
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("创建AVFormatContext失败"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("创建AVFormatContext成功"),
                              LogLevel::INFO);

  if (avformat_find_stream_info(formatContext, nullptr) < 0) {
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("检索流信息失败"), LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("检索流信息成功"), LogLevel::INFO);

  int videoStreamIndex = -1;
  for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
    if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      videoStreamIndex = i;
      break;
    }
  }

  if (videoStreamIndex == -1) {
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("未找到视频流"), LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(Decode::tr("视频解码"), Decode::tr("找到视频流"),
                              LogLevel::INFO);

  int audioStreamIndex = -1;
  for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
    if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      audioStreamIndex = i;
      break;
    }
  }

  if (audioStreamIndex == -1) {
    //    qDebug() << "No audio stream found";
    return;
  }

  AVCodecParameters *vCodecParameters =
      formatContext->streams[videoStreamIndex]->codecpar;
  AVCodecParameters *aCodecParameters =
      formatContext->streams[audioStreamIndex]->codecpar;

  // const AVCodec *videoCodec = avcodec_find_decoder_by_name("h264_qsv");
  const AVCodec *videoCodec = avcodec_find_decoder(vCodecParameters->codec_id);
  const AVCodec *audioCodec = avcodec_find_decoder(aCodecParameters->codec_id);

  if (!videoCodec) {
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("未找到编解码器"), LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("找到编解码器"), LogLevel::INFO);

  if (!audioCodec) {
    Logger::getInstance().log(Decode::tr("音频解码"),
                              Decode::tr("未找到编解码器"), LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(Decode::tr("音频解码"),
                              Decode::tr("找到编解码器"), LogLevel::INFO);

  AVCodecContext *videoCodecContext = avcodec_alloc_context3(videoCodec);
  AVCodecContext *audioCodecContext = avcodec_alloc_context3(audioCodec);

  avcodec_parameters_to_context(videoCodecContext, vCodecParameters);
  avcodec_parameters_to_context(audioCodecContext, aCodecParameters);

  if (avcodec_open2(videoCodecContext, videoCodec, nullptr) < 0) {
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("创建AVCodecContext失败"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(Decode::tr("视频解码"),
                              Decode::tr("创建AVCodecContext成功"),
                              LogLevel::INFO);

  if (avcodec_open2(audioCodecContext, audioCodec, nullptr) < 0) {
    Logger::getInstance().log(Decode::tr("音频解码"),
                              Decode::tr("创建AVCodecContext失败"),
                              LogLevel::INFO);
    return;
  } else
    Logger::getInstance().log(Decode::tr("音频解码"),
                              Decode::tr("创建AVCodecContext成功"),
                              LogLevel::INFO);

  videoCodecContext->thread_count = 0;  // 0 for auto
  videoCodecContext->thread_type = FF_THREAD_SLICE | FF_THREAD_FRAME;

  audioCodecContext->thread_count = 0;  // 0 for auto
  audioCodecContext->thread_type = FF_THREAD_SLICE | FF_THREAD_FRAME;

  AVStream **stream = formatContext->streams;
  int den = (*stream)->avg_frame_rate.den;
  int num = (*stream)->avg_frame_rate.num;
  float rate = (float)num / (float)den;
  int freq = 1000.0 / rate;

  g_freq.store(freq);

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

  m_audioDecode.setCodecContext(audioCodecContext);
  m_audioDecode.setAudioSpec(&audioSpec);
  m_audioDecode.setswrCtx(swrCtx);

  m_videoDecode.setCodecContext(videoCodecContext);

  g_audioCodecContext = audioCodecContext;
  g_audioStream = formatContext->streams[audioStreamIndex];
  g_videoStream = formatContext->streams[videoStreamIndex];

  m_audioDecode.start();
  m_videoDecode.start();
  m_syncThread->start();

  //  using namespace std::chrono;

  //  auto start = high_resolution_clock::now();

  //  size_t apacket = 0;
  //  size_t vpacket = 0;

  AVPacket *packet{0};
  while ((packet = packet_pool.allocate_packet()) &&
         av_read_frame(formatContext, packet) >= 0) {
    //    auto now = high_resolution_clock::now();
    //    auto duration = duration_cast<milliseconds>(now - start).count();
    if (packet->stream_index == audioStreamIndex) {
      // apacket++;
      audioPacketQueue.push(packet);
    } else if (packet->stream_index == videoStreamIndex) {
      // vpacket++;
      videoPacketQueue.push(packet);
    }

    //    if (duration >= 10000) {
    //      qDebug() << "ap" << apacket << "vp" << vpacket;
    //      apacket = 0;
    //      vpacket = 0;
    //      start = now;
    //    }

    if (m_stopFlag.loadAcquire()) break;
    QThread::usleep(1);
  }

  m_audioDecode.setStopFlag();
  m_audioDecode.wait();

  m_videoDecode.setStopFlag();
  m_videoDecode.wait();

  m_syncThread->setStopFlag();
  m_syncThread->wait();

  avcodec_close(videoCodecContext);
  avcodec_close(audioCodecContext);

  avcodec_free_context(&videoCodecContext);
  avcodec_free_context(&audioCodecContext);

  avformat_close_input(&formatContext);
  avformat_free_context(formatContext);
}

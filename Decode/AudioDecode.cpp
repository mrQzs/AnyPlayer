
#include "AudioDecode.h"

#include "Logger.hpp"

AudioDecode &AudioDecode::getInstance() {
  static AudioDecode instance;
  return instance;
}

void AudioDecode::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void AudioDecode::setCodecContext(AVCodecContext *audioCodecContex) {
  m_audioCodecContext = audioCodecContex;
}

void AudioDecode::setAudioSpec(SDL_AudioSpec *audioSpec) {
  m_audioSpec = audioSpec;
}

void AudioDecode::setswrCtx(SwrContext *swrCtx) { m_swrCtx = swrCtx; }

void AudioDecode::run() {
  Logger::getInstance().log(AudioDecode::tr("线程"),
                            AudioDecode::tr("音频解码线程启动"),
                            LogLevel::INFO);

  AVPacket packet;
  while (!m_stopFlag.loadAcquire()) {
    if (audioPacketQueue.pop(packet)) {
      AVFrame *aframe = av_frame_alloc();
      avcodec_send_packet(m_audioCodecContext, &packet);
      if (avcodec_receive_frame(m_audioCodecContext, aframe) == 0) {
        AVFrame *resampledFrame = av_frame_alloc();
        resampledFrame->channel_layout =
            av_get_default_channel_layout(m_audioSpec->channels);
        resampledFrame->format = AV_SAMPLE_FMT_S16;
        resampledFrame->sample_rate = m_audioSpec->freq;
        resampledFrame->channels = m_audioSpec->channels;

        if (swr_convert_frame(m_swrCtx, resampledFrame, aframe) < 0) {
          fprintf(stderr, "swr_convert_frame() failed\n");
          av_frame_free(&resampledFrame);
        } else {
          std::lock_guard<std::mutex> lock(mutex);
          Q_UNUSED(lock);
          audioFrameQueue.push(resampledFrame);
        }
      } else
        av_frame_free(&aframe);

      av_packet_unref(&packet);
    } else
      std::this_thread::yield();
  }

  Logger::getInstance().log(AudioDecode::tr("线程"),
                            AudioDecode::tr("音频解码线程释放剩余的队列"),
                            LogLevel::INFO);

  while (audioPacketQueue.pop(packet)) {
    av_packet_unref(&packet);
  }

  Logger::getInstance().log(AudioDecode::tr("线程"),
                            AudioDecode::tr("音频解码线程结束"),
                            LogLevel::INFO);
}

AudioDecode::AudioDecode() {}

AudioDecode::~AudioDecode() {}

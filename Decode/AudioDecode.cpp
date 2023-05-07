
#include "AudioDecode.h"

#include <QAtomicInt>

#include "Logger.h"

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

  m_stopFlag.storeRelaxed(0);
  AVPacket *packet{0};
  AVFrame *aframe{0};
  while (!m_stopFlag.loadAcquire()) {
    if (!audioPacketQueue.empty()) {
      audioPacketQueue.pop(packet);
      AVFrame *aframe = audio_frame_pool.allocate();
      avcodec_send_packet(m_audioCodecContext, packet);
      if (avcodec_receive_frame(m_audioCodecContext, aframe) == 0) {
        AVFrame *resampledFrame = audio_frame_pool.allocate();
        resampledFrame->channel_layout =
            av_get_default_channel_layout(m_audioSpec->channels);
        resampledFrame->format = AV_SAMPLE_FMT_S16;
        resampledFrame->sample_rate = m_audioSpec->freq;
        resampledFrame->channels = m_audioSpec->channels;

        if (swr_convert_frame(m_swrCtx, resampledFrame, aframe) < 0) {
          fprintf(stderr, "swr_convert_frame() failed\n");
          audio_frame_pool.release(resampledFrame);
        } else {
          resampledFrame->pts = aframe->pts;
          resampledFrame->nb_samples = aframe->nb_samples;
          resampledFrame->pkt_duration = aframe->pkt_duration;
          resampledFrame->pkt_pos = aframe->pkt_pos;

          audioFrameQueue.push(resampledFrame);
        }
      }

      audio_frame_pool.release(aframe);
      packet_pool.release_packet(packet);
    } else {
      QThread::usleep(10);
    }
  }

  SDL_CloseAudio();
  SDL_Quit();

  Logger::getInstance().log(AudioDecode::tr("线程"),
                            AudioDecode::tr("音频解码线程释放剩余的队列"),
                            LogLevel::INFO);

  while (!audioFrameQueue.empty()) {
    audioFrameQueue.pop(aframe);
    if (aframe) audio_frame_pool.release(aframe);
  }

  while (!audioPacketQueue.empty()) {
    audioPacketQueue.pop(packet);
    packet_pool.release_packet(packet);
  }

  Logger::getInstance().log(AudioDecode::tr("线程"),
                            AudioDecode::tr("音频解码线程结束"),
                            LogLevel::INFO);
}

AudioDecode::AudioDecode() {}

AudioDecode::~AudioDecode() {}

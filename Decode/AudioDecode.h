
#ifndef AUDIODECODE_H
#define AUDIODECODE_H

#include <QThread>

#include "GlobalVar.h"

class QAtomicInt;

class AudioDecode : public QThread {
 public:
  static AudioDecode &getInstance();

 public:
  void setStopFlag();
  void setCodecContext(AVCodecContext *audioCodecContex);
  void setAudioSpec(SDL_AudioSpec *audioSpec);
  void setswrCtx(SwrContext *swrCtx);

 protected:
  void run();

 private:
  AudioDecode();
  ~AudioDecode();
  AudioDecode(const AudioDecode &) = delete;
  AudioDecode &operator=(const AudioDecode &) = delete;

 private:
  QAtomicInt m_stopFlag;
  AVCodecContext *m_audioCodecContext;
  SDL_AudioSpec *m_audioSpec;
  SwrContext *m_swrCtx;
};

#endif  // AUDIODECODE_H

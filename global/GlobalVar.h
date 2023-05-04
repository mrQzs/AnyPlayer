
#ifndef GLOBALVAR_H
#define GLOBALVAR_H

#include <QAtomicInt>
#include <QImage>
#include <atomic>
#include <chrono>
#include <iostream>

extern "C" {
#include <SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include "CVQueue.h"

constexpr size_t QUEUESIZE = 128;

extern CVQueue<AVPacket> audioPacketQueue;
extern CVQueue<AVFrame*> audioFrameQueue;
extern CVQueue<AVPacket> videoPacketQueue;
extern CVQueue<AVFrame*> videoFrameQueue;

extern std::atomic<double> audioPts;
extern std::atomic<double> videoPts;

extern AVCodecContext* g_audioCodecContext;
extern AVStream* g_videoStream;
extern AVStream* g_audioStream;

extern std::atomic<int> g_freq;

#endif  // GLOBALVAR_H

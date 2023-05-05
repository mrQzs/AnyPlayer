
#ifndef GLOBALVAR_H
#define GLOBALVAR_H

#include <QAtomicInt>
#include <QImage>
#include <atomic>
#include <boost/pool/object_pool.hpp>
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

#include "LFQueue.h"

constexpr size_t QUEUESIZE = 128;

extern boost::object_pool<AVPacket> packet_pool;

extern LFQueue<AVPacket*> audioPacketQueue;
extern LFQueue<AVFrame*> audioFrameQueue;
extern LFQueue<AVPacket*> videoPacketQueue;
extern LFQueue<AVFrame*> videoFrameQueue;

extern std::atomic<double> audioPts;
extern std::atomic<double> videoPts;

extern AVCodecContext* g_audioCodecContext;
extern AVStream* g_videoStream;
extern AVStream* g_audioStream;

extern std::atomic<int> g_freq;

AVPacket* allocate_packet();
void release_packet(AVPacket* packet);

void audio_callback(void* userdata, Uint8* stream, int len);

#endif  // GLOBALVAR_H

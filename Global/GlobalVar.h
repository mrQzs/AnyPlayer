
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

#include "FramePool.h"
#include "LFQueue.h"
#include "PacketPool.h"

extern PacketPool packet_pool;
extern FramePool audio_frame_pool;
extern FramePool video_frame_pool;

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

void audio_callback(void* userdata, Uint8* stream, int len);

extern SwsContext* swsCtx;
extern size_t frameW;
extern size_t widgetW;
extern size_t frameH;
extern size_t widgetH;
extern AVFrame* rgbFrame;

QImage avFrameToQImage(AVFrame* frame);

#endif  // GLOBALVAR_H

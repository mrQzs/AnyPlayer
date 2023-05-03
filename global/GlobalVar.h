
#ifndef GLOBALVAR_H
#define GLOBALVAR_H

#include <QAtomicInt>
#include <QImage>
#include <mutex>
#include <queue>

#include "boost/lockfree/queue.hpp"
#include "boost/lockfree/spsc_queue.hpp"

extern "C" {
#include <SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

constexpr size_t QUEUE_SIZE = 2048;
extern boost::lockfree::spsc_queue<AVPacket> audioPacketQueue;
extern std::queue<AVFrame*> audioFrameQueue;
extern boost::lockfree::spsc_queue<AVPacket> videoPacketQueue;
extern boost::lockfree::spsc_queue<QImage*> imageQueue;

extern std::mutex mutex;

#endif  // GLOBALVAR_H

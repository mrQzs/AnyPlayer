
#ifndef GLOBALVAR_H
#define GLOBALVAR_H

#include <QAtomicInt>
#include <QImage>
#include <atomic>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

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

template <typename T>
class BoundedQueue {
 public:
  BoundedQueue(size_t maxSize)
      : maxSize(maxSize), itemCount(0), freeSlots(maxSize) {}

  void push(const T& value) {
    freeSlots.wait();
    std::unique_lock<std::mutex> lock(queueMutex);
    queue.push(value);
    itemCount.post();
  }

  bool pop(T& value) {
    itemCount.wait();
    std::unique_lock<std::mutex> lock(queueMutex);
    value = queue.front();
    queue.pop();
    freeSlots.post();
    return true;
  }

  bool empty() const {
    std::unique_lock<std::mutex> lock(queueMutex);
    return queue.empty();
  }

  size_t size() const {
    std::unique_lock<std::mutex> lock(queueMutex);
    return queue.size();
  }

 private:
  std::queue<T> queue;
  size_t maxSize;
  mutable std::mutex queueMutex;
  boost::interprocess::interprocess_semaphore itemCount;
  boost::interprocess::interprocess_semaphore freeSlots;
};

constexpr size_t QUEUESIZE = 256;

extern BoundedQueue<AVPacket> audioPacketQueue;
extern BoundedQueue<AVFrame*> audioFrameQueue;
extern BoundedQueue<AVPacket> videoPacketQueue;
extern BoundedQueue<AVFrame*> videoFrameQueue;

extern std::atomic<double> audioPts;
extern std::atomic<double> videoPts;

extern AVStream* g_videoStream;
extern AVStream* g_audioStream;

extern std::atomic<int> g_freq;

#endif  // GLOBALVAR_H

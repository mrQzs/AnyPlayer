
#ifndef FRAMEm_poolH
#define FRAMEm_poolH

extern "C" {
#include <libavutil/frame.h>
}

#include <mutex>
#include <stack>

class FramePool {
 public:
  FramePool(size_t initial_size) {
    for (size_t i = 0; i < initial_size; ++i) {
      m_pool.push(av_frame_alloc());
    }
  }

  ~FramePool() {
    while (!m_pool.empty()) {
      AVFrame *frame = m_pool.top();
      m_pool.pop();
      av_frame_free(&frame);
    }
  }

  AVFrame *allocate() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_pool.empty()) {
      return av_frame_alloc();
    } else {
      AVFrame *frame = m_pool.top();
      m_pool.pop();
      return frame;
    }
  }

  void release(AVFrame *frame) {
    if (frame) {
      av_frame_unref(frame);
      std::lock_guard<std::mutex> lock(m_mutex);
      m_pool.push(frame);
    }
  }

 private:
  std::stack<AVFrame *> m_pool;
  std::mutex m_mutex;
};

#endif  // FRAMEm_poolH


#ifndef LFQUEUE_H
#define LFQUEUE_H

#include <boost/lockfree/spsc_queue.hpp>
#include <chrono>
#include <thread>

template <typename T>
class LFQueue {
 public:
  LFQueue(size_t maxSize) : queue(maxSize) {}

  void push(const T& value) {
    while (!queue.push(value)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  bool pop(T& value) { return queue.pop(value); }

  bool empty() const { return queue.read_available() == 0; }

  size_t size() const { return queue.read_available(); }

 private:
  boost::lockfree::spsc_queue<T> queue;
};

#endif  // LFQUEUE_H


#ifndef CVQUEUE_H
#define CVQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class CVQueue {
 public:
  CVQueue(size_t maxSize) : maxSize(maxSize) {}

  void push(const T& value) {
    std::unique_lock<std::mutex> lock(queueMutex);
    notFull.wait(lock, [this]() { return queue.size() < maxSize; });
    queue.push(value);
    notEmpty.notify_one();
  }

  bool pop(T& value) {
    std::unique_lock<std::mutex> lock(queueMutex);
    notEmpty.wait(lock, [this]() { return !queue.empty(); });
    value = queue.front();
    queue.pop();
    notFull.notify_one();
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
  std::condition_variable notEmpty;
  std::condition_variable notFull;
};
#endif  // CVQUEUE_H

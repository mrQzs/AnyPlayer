
#ifndef ISQUEUE_H
#define ISQUEUE_H

#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <mutex>
#include <queue>

template <typename T>
class ISQueue {
 public:
  ISQueue(size_t maxSize);

 public:
  ISQueue(size_t maxSize)
      : maxSize(maxSize), itemCount(0), freeSlots(maxSize) {}

  void push(const T &value) {
    freeSlots.wait();
    std::unique_lock<std::mutex> lock(queueMutex);
    queue.push(value);
    itemCount.post();
  }

  bool pop(T &value) {
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

#endif  // ISQUEUE_H


#ifndef HELPER_H
#define HELPER_H

#include <QAtomicInt>

class Helper {
 public:
  static Helper& getInstance();

 public:
  void setSecFreq(int freq);
  int getSecFreq();

 private:
  Helper();
  ~Helper();

  Helper(const Helper&) = delete;
  Helper& operator=(const Helper&) = delete;

 private:
  QAtomicInt m_secFreq;
};

#endif  // HELPER_H

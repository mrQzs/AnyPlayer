
#ifndef UPATETIMER_H
#define UPATETIMER_H

#include "BaseThread.h"

class UpateTimer : public BaseThread {
  Q_OBJECT

 public:
  static UpateTimer &getInstance();

 protected:
  void run();

 signals:
  void timeout();

 private:
  UpateTimer();
  ~UpateTimer();
  UpateTimer(const UpateTimer &) = delete;
  UpateTimer &operator=(const UpateTimer &) = delete;
};

#endif  // UPATETIMER_H

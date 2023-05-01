
#include "Helper.h"

Helper &Helper::getInstance() {
  static Helper instance;
  return instance;
}

void Helper::setSecFreq(int freq) { m_secFreq.storeRelaxed(freq); }

int Helper::getSecFreq() { return m_secFreq.loadAcquire(); }

Helper::Helper() : m_secFreq(41) {}

Helper::~Helper() {}

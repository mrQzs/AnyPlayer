
#include "AudioDecode.h"

void AudioDecode::test() {}

AudioDecode::AudioDecode() {}

AudioDecode &AudioDecode::getInstance() {
  static AudioDecode instance;
  return instance;
}

AudioDecode::~AudioDecode() {}

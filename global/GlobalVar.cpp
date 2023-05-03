
#include "GlobalVar.h"

BoundedQueue<AVPacket> audioPacketQueue{QUEUESIZE};
BoundedQueue<AVFrame*> audioFrameQueue{QUEUESIZE * 2};
BoundedQueue<AVPacket> videoPacketQueue{QUEUESIZE};
BoundedQueue<AVFrame*> videoFrameQueue{QUEUESIZE * 2};

std::atomic<double> audioPts{0};
std::atomic<double> videoPts{0};

AVStream* g_videoStream = nullptr;
AVStream* g_audioStream = nullptr;

std::atomic<int> g_freq{41};


#include "GlobalVar.h"

CVQueue<AVPacket> audioPacketQueue{QUEUESIZE};
CVQueue<AVFrame*> audioFrameQueue{QUEUESIZE * 2};
CVQueue<AVPacket> videoPacketQueue{QUEUESIZE};
CVQueue<AVFrame*> videoFrameQueue{QUEUESIZE * 2};

std::atomic<double> audioPts{0};
std::atomic<double> videoPts{0};

AVCodecContext* g_audioCodecContext = nullptr;
AVStream* g_videoStream = nullptr;
AVStream* g_audioStream = nullptr;

std::atomic<int> g_freq{41};

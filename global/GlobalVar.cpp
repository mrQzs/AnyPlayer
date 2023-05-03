
#include "GlobalVar.h"

boost::lockfree::spsc_queue<AVPacket> audioPacketQueue{QUEUE_SIZE};
std::queue<AVFrame*> audioFrameQueue;
boost::lockfree::spsc_queue<AVPacket> videoPacketQueue{QUEUE_SIZE};
boost::lockfree::spsc_queue<QImage*> imageQueue{QUEUE_SIZE};

std::mutex mutex;

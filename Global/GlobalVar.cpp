
#include "GlobalVar.h"

boost::object_pool<AVPacket> packet_pool;
boost::object_pool<AVFrame> frame_pool;

LFQueue<AVPacket*> audioPacketQueue{QUEUESIZE};
LFQueue<AVFrame*> audioFrameQueue{QUEUESIZE * 2};
LFQueue<AVPacket*> videoPacketQueue{QUEUESIZE};
LFQueue<AVFrame*> videoFrameQueue{QUEUESIZE * 2};

std::atomic<double> audioPts{0};
std::atomic<double> videoPts{0};

AVCodecContext* g_audioCodecContext = nullptr;
AVStream* g_videoStream = nullptr;
AVStream* g_audioStream = nullptr;

std::atomic<int> g_freq{41};

AVPacket* allocate_packet() {
  AVPacket* packet = packet_pool.malloc();
  return packet;
}

void release_packet(AVPacket* packet) {
  if (packet) {
    av_packet_unref(packet);
    packet_pool.free(packet);
  }
}

void audio_callback(void* userdata, Uint8* stream, int len) {
  int bufferSize = 0;
  uint8_t* buffer = nullptr;
  AVFrame* frame = nullptr;

  while (len > 0) {
    if (bufferSize == 0) {
      {
        if (audioFrameQueue.empty()) {
          return;
        }
        audioFrameQueue.pop(frame);
      }

      if (frame) {
        audioPts = frame->pts * av_q2d(g_audioStream->time_base);
        int numSamples =
            av_get_bytes_per_sample(g_audioCodecContext->sample_fmt);
        bufferSize =
            frame->nb_samples * numSamples * g_audioCodecContext->channels;
        buffer = frame->data[0];
      }
    }

    int bytesToCopy = std::min(len, bufferSize);
    memcpy(stream, buffer, bytesToCopy);
    stream += bytesToCopy;
    buffer += bytesToCopy;
    bufferSize -= bytesToCopy;
    len -= bytesToCopy;
    if (frame) av_frame_free(&frame);
  }
}

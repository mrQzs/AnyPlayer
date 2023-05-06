
#include "GlobalVar.h"

constexpr size_t FrameSize = 200;
constexpr size_t QUEUESIZE = 94;  // audio  47 packets/sec

PacketPool packet_pool;
FramePool audio_frame_pool{FrameSize};
FramePool video_frame_pool{FrameSize};

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
    if (frame) audio_frame_pool.release(frame);
  }
}

SwsContext* swsCtx = nullptr;
size_t frameW{0};
size_t widgetW{0};
size_t frameH{0};
size_t widgetH{0};
AVFrame* rgbFrame = av_frame_alloc();

QImage avFrameToQImage(AVFrame* frame) {
  int w = frame->width;
  int h = frame->height;

  if (frameW == 0 || frameH == 0 || w != frameW || h != frameH) {
    // 获取 AVFrame 的宽度和高度
    frameW = frame->width;
    frameH = frame->height;

    // 创建一个 SwsContext，用于将 AVFrame 的像素格式转换为 QImage 支持的格式
    swsCtx = sws_getContext(frameW, frameH, (AVPixelFormat)frame->format,
                            frameW, frameH, AV_PIX_FMT_RGB32, SWS_BILINEAR,
                            NULL, NULL, NULL);
  }

  if (!swsCtx) {
    // 如果无法创建 SwsContext，请返回一个空 QImage
    return QImage();
  }

  // 分配一个新的 AVFrame 用于存储转换后的像素数据
  rgbFrame->width = frameW;
  rgbFrame->height = frameH;
  rgbFrame->format = AV_PIX_FMT_RGB32;

  // 为 rgbFrame 分配缓冲区
  int ret = av_frame_get_buffer(rgbFrame, 1);
  if (ret < 0) {
    // 如果无法为 AVFrame 分配缓冲区，请返回一个空 QImage
    av_frame_unref(rgbFrame);
    return QImage();
  }

  // 使用 sws_scale 将原始 AVFrame 的像素格式转换为 AV_PIX_FMT_RGB32
  sws_scale(swsCtx, frame->data, frame->linesize, 0, frameH, rgbFrame->data,
            rgbFrame->linesize);

  // 使用转换后的像素数据创建一个 QImage
  QImage image(rgbFrame->data[0], frameW, frameH, rgbFrame->linesize[0],
               QImage::Format_RGB32);

  QImage result = image.scaled(QSize(widgetW, widgetH), Qt::IgnoreAspectRatio,
                               Qt::SmoothTransformation);
  av_frame_unref(rgbFrame);

  return result;
}


#include "PacketPool.h"

PacketPool::PacketPool() {}

AVPacket *PacketPool::allocate_packet() {
  AVPacket *packet = m_packet_pool.malloc();
  return packet;
}

void PacketPool::release_packet(AVPacket *packet) {
  if (packet) {
    av_packet_unref(packet);
    m_packet_pool.free(packet);
  }
}

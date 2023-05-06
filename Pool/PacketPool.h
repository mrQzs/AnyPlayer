
#ifndef PACKETPOOL_H
#define PACKETPOOL_H

extern "C" {
#include <libavcodec/packet.h>
}

#include <boost/pool/object_pool.hpp>

class PacketPool {
 public:
  PacketPool();

 public:
  AVPacket* allocate_packet();
  void release_packet(AVPacket* packet);

 private:
  boost::object_pool<AVPacket> m_packet_pool;
};

#endif  // PACKETPOOL_H

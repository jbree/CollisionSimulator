#include "Packet.hh"

const std::map<PacketType, size_t> Packet::PACKET_SIZE {
    { PacketType::Ack, 30 },
    { PacketType::Data, 1500 },
    { PacketType::RTS, 30 },
    { PacketType::CTS, 30 },
};
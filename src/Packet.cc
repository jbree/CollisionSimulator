#include "Packet.hh"

const std::map<PacketType, size_t> Packet::PACKET_SIZE {
    { PacketType::Ack, 30 },
    { PacketType::Data, 1500 },
    { PacketType::RTS, 30 },
    { PacketType::CTS, 30 },
};

bool Packet::operator== (const Packet& rhs) const
{
    if (this == &rhs) {
        return true;
    }

    return (src == rhs.src 
            && dst == rhs.dst
            && type == rhs.type
            && size == rhs.size);
}
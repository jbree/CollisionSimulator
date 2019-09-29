#pragma once

#include <cstdlib>
#include <map>
#include <string>

typedef uint32_t Id;

enum class PacketType {
    Ack,
    Data,
    RTS,
    CTS,
};

struct Packet {
    std::string src;
    std::string dst;

    PacketType type;

    size_t size;

    bool operator== (const Packet& rhs) const;

    static const std::map<PacketType, size_t> PACKET_SIZE;
};

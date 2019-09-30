#pragma once

#include <cstdlib>
#include <iosfwd>
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

    friend std::ostream& operator<< (std::ostream& out, const Packet& rhs);

    static const std::map<PacketType, size_t> PACKET_SIZE;
};

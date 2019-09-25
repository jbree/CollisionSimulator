#pragma once

#include <cstdint>
#include <cstdlib>

typedef uint32_t Id;

enum class PacketType {
    Ack,
    Data
};

struct Packet {
    Id src;
    Id dst;

    PacketType type;

    size_t size;
};
#pragma once

#include "Station.hh"
#include <cstdint>
#include <list>
#include <string>


class Medium;
class Packet;

class ReceiverStation: public Station {

public:
    /// Construct a station, which transmits across the provided media.
    ReceiverStation (
            const std::string& name,
            const std::set<std::shared_ptr<Medium>>& media
            );

    /// Invoke this method to indicate the beginning of a time slot
    void tick () override;

    /// Invoke this method to indicate the end of a time slot
    void tock () override;

    /// This function is invoked by a Medium when a packet is thrown in its
    /// general direction
    virtual void receive (const Packet& packet) override;

    /// @return The number of packets which collided (packets, not slots)
    virtual uint64_t collisions () const;

    /// @return The number of packets which were received and ACKed
    virtual uint64_t receivedPackets () const;

protected:

    enum class State {
        Receiving,
        Acking,
    };

    State state_;

    uint16_t ackTick_;
    std::list<Packet> receivingPackets_;
    std::unique_ptr<Packet> receivingPacket_;
    uint16_t receivedBytes_;

    bool colliding_;
    uint64_t collisions_;
    uint64_t receivedPacketCount_;

private:

};
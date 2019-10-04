#include "Medium.hh"
#include "Packet.hh"
#include "ReceiverStation.hh"
#include <iostream>


ReceiverStation::ReceiverStation (
        const std::string& name,
        const std::set<std::shared_ptr<Medium>>& media
        )
: Station(name, media)
, state_(State::Receiving)
, colliding_(false)
, collisions_(0)
, receivedPacketCount_(0)
{
}


uint64_t ReceiverStation::collisions () const
{
    return collisions_;
}


uint64_t ReceiverStation::receivedPackets () const
{
    return receivedPacketCount_;
}




void ReceiverStation::receive (const Packet& packet)
{
    receivingPackets_.push_back(packet);
}


void ReceiverStation::tick ()
{
    receivingPackets_.clear();
    
    switch (state_) {
    case State::Receiving:
        break;

    case State::Acking:
        if (ackTick_++) {
            Packet ack;
            ack.src = name_;
            ack.dst = receivingPacket_->src;
            ack.type = PacketType::Ack;
            ack.size = Packet::PACKET_SIZE.at(PacketType::Ack);

            transmitFragment(ack);
        }

        if (ackTick_ > 2) {
            receivedBytes_ = 0;
            receivingPacket_.reset();
            state_ = State::Receiving;
        }
        break;
    }

}

void ReceiverStation::tock ()
{
    bool busy(false);

    switch (state_) {
    case State::Receiving:
        // multiple or zero packets, reset and keep waiting
        if (receivingPackets_.empty()) {
            colliding_ = false;
            break;
        }
        if (receivingPackets_.size() > 1) {
            receivedBytes_ = 0;
            receivingPacket_.reset();
            if (!colliding_) {
                colliding_ = true;
                collisions_++;
                std::cout << "collision!" << std::endl;
            }
            break;
        }

        // ignore packets sent to a different receiver
        if (receivingPackets_.front().dst != name_) {
            break;
        }

        // first packet
        if (!receivingPacket_) {
            receivingPacket_ = std::unique_ptr<Packet>(
                    new Packet(receivingPackets_.front()));
            std::cout << name_ << " receiving packet: \n    "
                    << *receivingPacket_ << std::endl;

            receivedBytes_ = 0;
        }

        receivedBytes_ += BYTES_PER_TICK;
        
        if (receivedBytes_ == Packet::PACKET_SIZE.at(PacketType::Data)) {
            std::cout << name_ << " finished receiving packet:\n    "
                    << *receivingPacket_ << std::endl;
            ackTick_ = 0;
            receivedPacketCount_++;
            state_ = State::Acking;
        }
        break;

    case State::Acking:
        break;
    }
}

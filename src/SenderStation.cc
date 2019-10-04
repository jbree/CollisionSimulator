#include "Medium.hh"
#include "Packet.hh"
#include "SenderStation.hh"
#include <iostream>

static const uint16_t CONTENTION_WINDOW_MAX { 1024 };
static const uint16_t CONTENTION_WINDOW_DEFAULT { 4 };


SenderStation::SenderStation (
        const std::string& name,
        const std::set<std::shared_ptr<Medium>>& media,
        bool virtualCarrierSensingEnabled
        )
: Station(name, media)
, transmittedBytes_(0)
, media_(media)
, state_(State::Idle)
, contentionWindow_(CONTENTION_WINDOW_DEFAULT)
, virtualCarrierSensingEnabled_(false)
, ticks_(0)
{
}

/// A packet arrives for transmission
void SenderStation::arrive (const Packet& packet)
{
    std::cout << "packet arrived at " << name_
            << " (tick " << ticks_ << ")" << std::endl;
    arrivedPackets_.push_back(packet);
}

void SenderStation::receive (const Packet& packet)
{
    if (state_ == State::WaitForAck
            && packet.type == PacketType::Ack
            && rxPacketBelongsToUs(packet))
    {
        std::cout << name_ << " received ACK"
                << " (tick " << ticks_ << ")" << std::endl;

        // SUCCESS! we sent a packet, and it was acked.
        // remove it from the send list
        arrivedPackets_.pop_front();

        // either go to idle, or prepare to send another packet
        state_ = arrivedPackets_.empty() ? State::Idle : State::Ready;

        contentionWindow_ = CONTENTION_WINDOW_DEFAULT;

        return;
    }
}


void SenderStation::tick ()
{
    ticks_++;

    switch (state_) {
    case State::Idle:
        // No-op
        break;

    case State::Ready:
        // where do we set this back to default?
        backoff_ = random() % contentionWindow_;
        remainingSenseTicks_ = DIFS_TICKS;
    
        std::cout << "packet ready at " << name_
                << ", selected random backoff of " << backoff_ << std::endl;

        // When this slot ticks, we are Ready, but by tock we need to Sense
        state_ = State::Sense;
        break;

    case State::Sense:
    case State::Backoff:
        // No-op
        break;

    case State::Transmit:
        transmitFragment(arrivedPackets_.front());
        transmittedBytes_ += BYTES_PER_TICK;

        if (transmittedBytes_ == Packet::PACKET_SIZE.at(PacketType::Data)) {
            std::cout << name_ << " finished sending to "
                    << arrivedPackets_.front().dst
                    << " (tick " << ticks_ << ")" << std::endl;

            state_ = State::WaitForAck;
            waitForAckTicks_ = -1;
            transmittedBytes_ = 0;
        }
        break;

    case State::WaitForAck:
        //TODO
        break;
    }
}

void SenderStation::tock ()
{
    bool busy(false);

    switch (state_) {
    case State::Idle:
        if (!arrivedPackets_.empty()) {
            state_ = State::Ready;
        }
        break;

    case State::Ready:
        // No-op
        break;

    case State::Sense:
        if (mediaBusy()){
            remainingSenseTicks_ = DIFS_TICKS;
            break;
        }

        if (--remainingSenseTicks_ == 0) {
            // if backoff is zero, skip directly to transmit
            state_ = backoff_ ? State::Backoff : State::Transmit;
        }

        break;

    case State::Backoff:
        // Any busy channel means we don't get to decrement our backoff counter
        if (mediaBusy()) {
            remainingSenseTicks_ = DIFS_TICKS;
            state_ = State::Sense;
            break;
        } else {
            backoff_--;
        }

        if (backoff_ == 0) {
            state_ = State::Transmit;
            std::cout << name_ << " starting transmit"
                << " (tick " << ticks_ << ")" << std::endl;
        }

        break;

    case State::Transmit:
        //TODO At end of tranmission, this needs to update state
        break;

    case State::WaitForAck:
        if (waitForAckTicks_++ > MAX_ACK_TICKS) {
            std::cout << name_ << " received no ack, retrying" << std::endl; 
            // Collision has occurred. Adjust contention window and try again.
            expandContentionWindow();
            backoff_ = random() % contentionWindow_;
            remainingSenseTicks_ = DIFS_TICKS;
            state_ = State::Sense;
        }
        break;

    default:
        break;
    }
}


void SenderStation::expandContentionWindow ()
{
    uint16_t cw(contentionWindow_ * 2);

    contentionWindow_ = (cw > CONTENTION_WINDOW_MAX
            ? CONTENTION_WINDOW_MAX
            : cw);
}

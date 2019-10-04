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
{
}

/// A packet arrives for transmission
void SenderStation::arrive (const Packet& packet)
{
    std::cout << "packet arrived at " << name_ << std::endl;
    arrivedPackets_.push_back(packet);
}

void SenderStation::receive (const Packet& packet)
{
    if (state_ == State::WaitForAck
            && packet.type == PacketType::Ack
            && rxPacketBelongsToUs(packet))
    {
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
    // this only tracks arrived packets per slot
    arrivedPackets_.clear();

    switch (state_) {
    case State::Idle:
        // No-op
        break;

    case State::Ready:
        // where do we set this back to default?
        backoff_ = random() % contentionWindow_;
        remainingSenseTicks_ = DIFS_TICKS;
        busyDuringSense_ = false;
    
        std::cout << "packet ready at " << name_ << std::endl;

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
            std::cout << "transmission of " << transmittedBytes_ << " from "
                    << name_ << "complete" << std::endl;

            state_ = State::WaitForAck;
            waitForAckTicks_ = -1;
        }
        std::cout << "transmitted " << transmittedBytes_ << " from " << name_ << std::endl;
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
        if (!busyDuringSense_ && mediaBusy()){
            busyDuringSense_ = true;
        }

        if (--remainingSenseTicks_ == 0) {
            state_ = busyDuringSense_ ? State::Backoff : State::Transmit;
        }

        break;

    case State::Backoff:
        // Any busy channel means we don't get to decrement our backoff counter
        if (mediaBusy()) {
            busy = true;
        }

        if (!busy) {
            backoff_--;
        }

        if (backoff_ == 0) {
            state_ = State::Transmit;
        }

        break;

    case State::Transmit:
        //TODO At end of tranmission, this needs to update state
        break;

    case State::WaitForAck:
        std::cout << name_ << " waiting for ack " << name_ << std::endl;

        if (waitForAckTicks_++ > MAX_ACK_TICKS) {
            std::cout << name_ << " received no ack, retrying" << std::endl; 
            // Collision has occurred. Adjust contention window and try again.
            expandContentionWindow();
            backoff_ = random() % contentionWindow_;
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

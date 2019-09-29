#include "Medium.hh"
#include "Packet.hh"
#include "Station.hh"
#include <iostream>

static const uint16_t CONTENTION_WINDOW_MAX { 1024 };
static const uint16_t CONTENTION_WINDOW_DEFAULT { 4 };

static const uint16_t SIFS_TICKS { 1 };
static const uint16_t DIFS_TICKS { 4 };

///FIXME Find a more sensible way to implement this. Just looks wrong.
static const int16_t MAX_ACK_TICKS { 1 };

static const size_t ACK_BYTES { 30 };
static const size_t RTS_BYTES { 30 };
static const size_t CTS_BYTES { 30 };


Station::Station (
        const std::string& name,
        const std::set<std::shared_ptr<Medium>>& media,
        bool virtualCarrierSensingEnabled
        )
: name_(name)
, transmittedBytes_(0)
, media_(media)
, state_(State::Idle)
, contentionWindow_(CONTENTION_WINDOW_DEFAULT)
, virtualCarrierSensingEnabled_(false)
{
    srandom(time(nullptr));
}

/// A packet arrives for transmission
void Station::arrive (const Packet& packet)
{
    std::cout << "packet arrived at " << name_ << std::endl;
    arrivedPackets_.push_back(packet);
}

void Station::receive (const Packet& packet)
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

    // if we're not yet receiving anything, start receiving this packet
    if (receivingPacket_ == nullptr) {
        receivingPacket_ = std::unique_ptr<Packet>(new Packet(packet));

        // first slot gives this many bytes
        receivedBytes_ = BYTES_PER_TICK;
        state_ = State::Receiving;
    } else if (packet == *receivingPacket_) {
        receivedBytes_ += BYTES_PER_TICK;
    } else {
        // two rx's in a single slot... collision!
        receivedBytes_ = 0;
        receivingPacket_.reset();
    }
}


void Station::tick ()
{
    // this only tracks arrived packets per slot
    arrivedPackets_.clear();

    switch (state_) {
    case State::Idle:
        // No-op
        break;
            
    case State::Receiving:
        break;

    case State::Acking:
        // by skipping tick 0, we wait SIFS to ack.
        if (ackTick_++) {
            Packet ack;
            ack.dst = receivingPacket_->src;
            ack.src = name_;
            ack.size = Packet::PACKET_SIZE.at(PacketType::Ack);

            transmitFragment(ack);
            if (ackTick_ > 2) {
                state_ = State::Idle;
            }
        }

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

        if (transmitFragment(arrivedPackets_.front())) {
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

void Station::tock ()
{
    bool busy(false);

    switch (state_) {
    case State::Idle:
        if (!arrivedPackets_.empty()) {
            state_ = State::Ready;
        }
        break;

    case State::Receiving:
        if (receivedBytes_ == Packet::PACKET_SIZE.at(PacketType::Ack)) {
            state_ = State::Acking;
            ackTick_ = 0;
        }
        break;

    case State::Acking:
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


bool Station::mediaBusy () const
{
    if (std::any_of(
            media_.begin(), 
            media_.end(),
            [](std::shared_ptr<Medium> m) { return m->isBusy(); }))
    {
        return true;
    }

    return false;
}


bool Station::transmitFragment (const Packet& packet)
{
    for (auto medium: media_) {
        medium->transmit(packet);
    }

    transmittedBytes_ += BYTES_PER_TICK;

    if (transmittedBytes_ == packet.size) {
        return true;
    }

    return false;
}


bool Station::rxPacketBelongsToUs (const Packet& packet) const
{
    return (packet.dst == name_);
}


const std::string& Station::name () const
{
    return name_;
}


void Station::expandContentionWindow ()
{
    uint16_t cw(contentionWindow_ * 2);

    contentionWindow_ = (cw > CONTENTION_WINDOW_MAX
            ? CONTENTION_WINDOW_MAX
            : cw);
}

#include "Medium.hh"
#include "Packet.hh"
#include "Station.hh"
#include <chrono>

static const uint16_t CONTENTION_WINDOW_MAX { 1024 };
static const uint16_t CONTENTION_WINDOW_DEFAULT { 4 };

static const uint16_t SIFS_TICKS { 1 };
static const uint16_t DIFS_TICKS { 4 };

static const size_t ACK_BYTES { 30 };
static const size_t RTS_BYTES { 30 };
static const size_t CTS_BYTES { 30 };


Station::Station (
        const std::string& name,
        const std::set<std::shared_ptr<Medium>>& media,
        bool virtualCarrierSensingEnabled
        )
: media_(media)
, state_(Station::State::Idle)
, contentionWindow_(CONTENTION_WINDOW_DEFAULT)
, virtualCarrierSensingEnabled_(false)
{
    srandom(time(nullptr));
}

/// A packet arrives for transmission
void Station::arrive (const Packet& packet)
{
    arrivedPackets_.push_back(packet);
}

void Station::tick ()
{
    switch (state_) {
    case State::Idle:
        // No-op
        break;

    case State::Ready:
        // where do we set this back to default?
        backoff_ = random() % contentionWindow_;
        remainingSenseTicks_ = DIFS_TICKS;
        busyDuringSense_ = false;
        
        // When this slot ticks, we are Ready, but by tock we need to Sense
        state_ = State::Sense;
        break;

    case State::Sense:
    case State::Backoff:
        // No-op
        break;

    case State::Transmit:
        //TODO Transmit on the medium
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
        // No-op
        break;

    default:
        break;
    }
}


bool Station::mediaBusy ()
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

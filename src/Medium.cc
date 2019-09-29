#include "Medium.hh"
#include "Packet.hh"
#include "Station.hh"
#include <stdexcept>


Medium::Medium (
        const std::string& name
        )
: isTicking_(false)
, isTransmitting_(false)
{
}


Medium::~Medium ()
{
}


void Medium::transmit (const Packet& p)
{
    if (!isTicking_) {
        throw std::runtime_error("Medium must tick() before transmitting");
    }

    packets_.push_back(p);
}

/// Sense this medium and determine if it is busy.
/// @return True if busy, false otherwise
bool Medium::isBusy () const
{
    if (isTicking_) {
        throw std::runtime_error("Medium must tock() before busy check");        
    }
    return isTransmitting_;
}

/// Add a station to this medium. Keep weak pointers to prevent cyclic
/// dependency.
void Medium::addStation (std::weak_ptr<Station> receiver)
{
    receivers_.emplace_back(std::move(receiver));
}

/// Invoke at the beginning of a simulation frame
void Medium::tick ()
{
    isTicking_ = true;
    isTransmitting_ = false;

    packets_.clear();
}

/// Invoke at the end of a simulation frame
void Medium::tock ()
{
    isTicking_ = false;

    if (packets_.empty()) {
        isTransmitting_ = false;
        return;
    }

    for (auto weakReceiver: receivers_) {
        for (auto packet: packets_) {
            auto receiver = weakReceiver.lock();
            if (receiver && packet.src != receiver->name()) {
                receiver->receive(packet);
            }
        }
    }
    isTransmitting_ = true;
}

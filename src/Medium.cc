#include "Medium.hh"
#include "Packet.hh"
#include "Station.hh"

void Medium::transmit (const Packet& p)
{
    // for (auto weakReceiver: receivers_) {
    //     auto receiver = weakReceiver.lock();
    //     if (receiver && p.src != receiver->name()) {
    //         receiver->receive(p);
    //     } 
    // }
}

/// Sense this medium and determine if it is busy.
/// @return True if busy, false otherwise
bool Medium::isBusy ()
{
    //FIXME
    return true;
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
    
}

/// Invoke at the end of a simulation frame
void Medium::tock ()
{
    
}

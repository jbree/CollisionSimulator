#pragma once

#include "Simulatable.hh"
#include <set>

class Packet;
class Station;

class Medium: public Simulatable {

    /// Transmit a packet across this medium. All registered receivers will
    /// receive the packet and determine if it is useful to them.
    void transmit (const Packet& p);
    
    /// Sense this medium and determine if it is busy.
    /// @return True if busy, false otherwise
    bool isBusy ();

    /// Add a station to this medium. Keep weak pointers to prevent cyclic
    /// dependency.
    void addStation (std::weak_ptr<Station> receiver);

private:

    std::set<std::weak_ptr<Station>> receivers_;

};
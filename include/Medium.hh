#pragma once

#include "Simulatable.hh"
#include <list>
#include <string>
#include <vector>

class Packet;
class Station;

class Medium: public Simulatable {
public:

    Medium (
            const std::string& name
            );

    virtual ~Medium ();

    /// Transmit a packet across this medium. All registered receivers will
    /// receive the packet and determine if it is useful to them.
    ///
    /// `tick()` must be called before transmit to ensure that the frame is in
    /// the correct state.
    virtual void transmit (const Packet& p);
    
    /// Sense this medium and determine if it is busy. Sensing may only be
    /// performed between tock and tick.
    ///
    /// @return True if busy, false otherwise
    virtual bool isBusy () const;

    /// Add a station to this medium. Keep weak pointers to prevent cyclic
    /// dependency.
    virtual void addStation (std::weak_ptr<Station> receiver);

    /// Invoke at the beginning of a simulation frame
    void tick () override;

    /// Invoke at the end of a simulation frame
    void tock () override;

    const std::string& name () const;

private:
    bool isTicking_;
    bool isTransmitting_;
    std::vector<std::weak_ptr<Station>> receivers_;
    std::list<Packet> packets_;

    std::string name_;

};
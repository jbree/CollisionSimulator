#pragma once

#include "Packet.hh"
#include "Simulatable.hh"
#include <cstdint>
#include <list>
#include <set>
#include <string>


class Medium;

static const uint16_t SIFS_TICKS { 1 };
static const uint16_t DIFS_TICKS { 4 };
static const uint16_t ACK_TICKS { 2 };

///FIXME Find a more sensible way to implement this. Just looks wrong.
static const int16_t MAX_ACK_TICKS { 1 };

static const size_t ACK_BYTES { 30 };
static const size_t RTS_BYTES { 30 };
static const size_t CTS_BYTES { 30 };

class Station: public Simulatable {

public:
    /// Construct a station, which transmits across the provided media.
    Station (
            const std::string& name,
            const std::set<std::shared_ptr<Medium>>& media
            );

    /// This function is invoked by a Medium when a packet is thrown in its
    /// general direction
    virtual void receive (const Packet& packet) = 0;

    /// @return the name/id of this Station
    virtual const std::string& name () const;

protected:

    /// @return True if *any* of the media this station transmits on are busy
    bool mediaBusy () const;

    /// Transmit a fragment on all of the available media.
    /// @return True if this is the last fragment (packet complete)
    void transmitFragment (const Packet& packet);

    /// @return True if the packet was sent to this Station
    bool rxPacketBelongsToUs (const Packet& packet) const;

    void expandContentionWindow ();

    std::string name_;
    std::set<std::shared_ptr<Medium>> media_;
};
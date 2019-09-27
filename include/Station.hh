#pragma once

#include "Simulatable.hh"
#include <chrono>
#include <cstdint>
#include <list>
#include <set>
#include <string>


class Medium;
struct Packet;

class Station: public Simulatable {

public:
    /// Construct a station, which transmits across the provided media.
    Station (
            const std::string& name,
            const std::set<std::shared_ptr<Medium>>& media,
            bool virtualCarrierSensingEnabled
            );

    /// A packet arrives for transmission. This method should be called between
    /// `tick()` and `tock()`
    void arrive (const Packet& packet);

    /// Invoke this method to indicate the beginning of a time slot
    void tick () override;

    /// Invoke this method to indicate the end of a time slot
    void tock () override;

    /// This function is invoked by a Medium when a packet is thrown in its
    /// general direction
    void receive (const Packet& packet);

    const std::string& name () const;

protected:

    /// @return True if *any* of the media this station transmits on are busy
    bool mediaBusy ();

private:

    std::string name_;
    std::list<Packet> arrivedPackets_;
    std::set<std::shared_ptr<Medium>> media_;

    enum class State {
        Idle,
        Ready,
        Sense,
        Backoff,
        Transmit,
        WaitForAck
    };

    State state_;

    uint16_t remainingSenseTicks_;
    bool busyDuringSense_;

    uint16_t backoff_;
    uint16_t contentionWindow_; /// Contention window slot count

    bool virtualCarrierSensingEnabled_; /// Virtual carrier sensing

};
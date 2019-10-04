#pragma once

#include "Station.hh"
#include <cstdint>
#include <list>
#include <set>
#include <string>


class Medium;
class Packet;

class SenderStation: public Station {

public:
    /// Construct a station, which transmits across the provided media.
    SenderStation (
            const std::string& name,
            const std::set<std::shared_ptr<Medium>>& media,
            bool virtualCarrierSensingEnabled
            );

    /// A packet arrives for transmission. This method should be called between
    /// `tick()` and `tock()`
    virtual void arrive (const Packet& packet);

    /// Invoke this method to indicate the beginning of a time slot
    void tick () override;

    /// Invoke this method to indicate the end of a time slot
    void tock () override;

    /// This function is invoked by a Medium when a packet is thrown in its
    /// general direction
    virtual void receive (const Packet& packet) override;

protected:

    void expandContentionWindow ();

    std::set<std::shared_ptr<Medium>> media_;

    
private:

    std::list<Packet> arrivedPackets_;
    std::unique_ptr<Packet> transmittingPacket_;
    size_t transmittedBytes_;

    std::list<Packet> receivedPacket_;
    std::unique_ptr<Packet> receivingPacket_;
    size_t receivedBytes_;

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
    int16_t waitForAckTicks_;

    uint16_t ackTick_;

    uint16_t backoff_;
    uint16_t contentionWindow_; /// Contention window slot count

    uint32_t ticks_;

    bool virtualCarrierSensingEnabled_; /// Virtual carrier sensing

};
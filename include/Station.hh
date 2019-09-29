#pragma once

#include "Packet.hh"
#include "Simulatable.hh"
#include <cstdint>
#include <list>
#include <set>
#include <string>


class Medium;

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
    virtual void arrive (const Packet& packet);

    /// Invoke this method to indicate the beginning of a time slot
    void tick () override;

    /// Invoke this method to indicate the end of a time slot
    void tock () override;

    /// This function is invoked by a Medium when a packet is thrown in its
    /// general direction
    virtual void receive (const Packet& packet);

    const std::string& name () const;

protected:

    /// @return True if *any* of the media this station transmits on are busy
    bool mediaBusy () const;

    /// Transmit a fragment on all of the available media.
    /// @return True if this is the last fragment (packet complete)
    bool transmitFragment (const Packet& packet);

    /// @return True if the packet was sent to this Station
    bool rxPacketBelongsToUs (const Packet& packet) const;

    void expandContentionWindow ();

private:

    std::string name_;
    std::list<Packet> arrivedPackets_;
    std::unique_ptr<Packet> transmittingPacket_;
    size_t transmittedBytes_;

    std::list<Packet> receivedPacket_;
    std::unique_ptr<Packet> receivingPacket_;
    size_t receivedBytes_;
    std::set<std::shared_ptr<Medium>> media_;

    enum class State {
        Idle,
        Receiving,
        Acking,
        Ready,
        Sense,
        Backoff,
        Transmit,
        WaitForAck
    };

    State state_;

    uint16_t remainingSenseTicks_;
    bool busyDuringSense_;
    int16_t waitForAckTicks_;

    uint16_t ackTick_;

    uint16_t backoff_;
    uint16_t contentionWindow_; /// Contention window slot count

    bool virtualCarrierSensingEnabled_; /// Virtual carrier sensing

};
#pragma once

#include "Simulatable.hh"
#include <chrono>
#include <cstdint>
#include <list>
#include <set>
#include <memory>

class Medium;
struct Packet;

class Station: public Simulatable {

public:
    /// Construct a station, which transmits across the provided media.
    ///
    /// Generate a random starting contentionWindow_
    Station (std::set<std::shared_ptr<Medium>> media);

    /// A packet arrives for transmission
    void arrive (const Packet& packet);


private:
    std::list<Packet> arrivedPackets_;
    std::set<std::shared_ptr<Medium>> media_;

    uint16_t contentionWindow_; /// contention window slot count
};
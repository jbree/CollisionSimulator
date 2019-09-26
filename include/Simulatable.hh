#pragma once

#include <cstdint>

class Simulatable {

public:

    const uint64_t MICROSECONDS_PER_TICK { 10 };
    const uint64_t BYTES_PER_TICK { 15 };

    /// Begin a simulation tick
    virtual void tick () = 0;

    /// End a simulation tick
    virtual void tock () = 0;

};
#pragma once

#include <cstdint>

class Simulatable {

public:
    Simulatable ();

    /// Begin a simulation tick
    virtual void tick ();

    /// End a simulation tick
    virtual void tock ();

    /// @return The current simulation clock value (in ticks)
    uint64_t clock () const;

    /// @return True if the frame is ticking, false if frame is tocking
    bool active () const;

private:
    uint64_t clock_;
    bool active_;
};
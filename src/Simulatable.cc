#include "Simulatable.hh"
#include <stdexcept>

Simulatable::Simulatable ()
: clock_(0)
, active_(false)
{
}

void Simulatable::tick ()
{
    if (active_) {
        throw std::runtime_error("Tried to tick while ticking");
    }

    active_ = true;
}

void Simulatable::tock ()
{
    if (!active_) {
        throw std::runtime_error("Tried to tock while tocking");
    }

    active_ = false;
    clock_++;
}

uint64_t Simulatable::clock () const
{
    return clock_;
}

bool Simulatable::active () const
{
    return active_;
}
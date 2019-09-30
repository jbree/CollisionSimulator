#include "Medium.hh"
#include "Station.hh"


Station::Station (
        const std::string& name,
        const std::set<std::shared_ptr<Medium>>& media
        )
: name_(name)
, media_(media)
{
}

bool Station::mediaBusy () const
{
    if (std::any_of(
            media_.begin(), 
            media_.end(),
            [](std::shared_ptr<Medium> m) { return m->isBusy(); }))
    {
        return true;
    }

    return false;
}


void Station::transmitFragment (const Packet& packet)
{
    for (auto medium: media_) {
        medium->transmit(packet);
    }
}


bool Station::rxPacketBelongsToUs (const Packet& packet) const
{
    return (packet.dst == name_);
}


const std::string& Station::name () const
{
    return name_;
}

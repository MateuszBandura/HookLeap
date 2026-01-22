#pragma once
#include "game/Pickup.hpp"

class WinPickup : public Pickup
{
public:
    WinPickup(const sf::Texture& texture);
    
    void collect() override;
    bool shouldRemainVisible() const override { return true; }
};

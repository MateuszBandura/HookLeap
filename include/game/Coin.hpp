#pragma once
#include "game/Pickup.hpp"

class Coin : public Pickup
{
public:
    Coin(const sf::Texture& texture);
    
    void collect() override;
    bool shouldRemainVisible() const override { return false; }
};

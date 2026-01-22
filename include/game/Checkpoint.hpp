#pragma once
#include "game/Pickup.hpp"

class Checkpoint : public Pickup
{
public:
    Checkpoint(const sf::Texture& texture);
    
    void collect() override;
    void animate(const sf::Time& elapsed) override;
    bool shouldRemainVisible() const override { return true; }
    
private:
    bool activated;
};

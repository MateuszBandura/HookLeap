#include "game/Checkpoint.hpp"

Checkpoint::Checkpoint(const sf::Texture& texture)
    : Pickup(texture), activated(false)
{
    // Setup checkpoint animation (assuming 6 frames)
    for (int i = 0; i < 6; ++i)
    {
        addAnimationFrame(sf::IntRect({i * 32, 0}, {32, 32}));
    }
    setFps(10);
    
    // Start at first frame (inactive) - IMPORTANT: Actually set the texture rect!
    setFrame(0);
    if (!animationFrames.empty())
    {
        setTextureRect(animationFrames[0]);
    }
}

void Checkpoint::collect()
{
    if (!activated)
    {
        Pickup::collect();
        activated = true;
    }
}

void Checkpoint::animate(const sf::Time& elapsed)
{
    if (!activated)
    {
        // Don't animate until activated
        return;
    }
    
    // Animate once, then stay at last frame
    if (frameId < animationFrames.size() - 1)
    {
        timeout += elapsed.asSeconds();
        
        if (timeout >= 1.0f / fps)
        {
            frameId++;
            timeout = 0;
        }
        
        setTextureRect(animationFrames[frameId]);
    }
}
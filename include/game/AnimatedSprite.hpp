#pragma once
#include <SFML/Graphics.hpp>

class AnimatedSprite : public sf::Sprite
{
public:
    using sf::Sprite::Sprite;

    void animate(const sf::Time &elapsed);
    void addAnimationFrame(const sf::IntRect &animationFrame);

    void setFps(int fps_);
    void setFrame(int frameId_);

    int getFrameId();
    int getFramesSize();

protected:
    std::vector<sf::IntRect> animationFrames;
    unsigned long long int frameId = 0;
    float timeout = 0;
    int fps = 1;
};
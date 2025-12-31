#include "game/Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

Player::Player(const sf::Texture& texture)
    : Character(texture, 100.0f), score(0), onGround(false), wasOnGround(false),
      currentState(PlayerState::Idle), currentDirection(Direction::Right)
{
}

void Player::setAnimationRow(PlayerState state, int row, int frameCount, bool shouldLoop)
{
    AnimationData animData;
    animData.row = row;
    animData.frameCount = frameCount;
    animData.loop = shouldLoop;
    
    // Create frames for this animation
    animData.frames.clear();
    for (int i = 0; i < frameCount; ++i)
    {
        animData.frames.push_back(sf::IntRect({i * 128, row * 128}, {128, 128}));
    }
    
    animations[state] = animData;
}

void Player::changeState(PlayerState newState)
{
    if (currentState != newState)
    {
        currentState = newState;
        frameId = 0; // Reset animation to first frame
        timeout = 0;
    }
}

void Player::updateDirection()
{
    sf::Vector2f vel = getVelocity();
    
    // Only change direction when actually moving horizontally
    if (vel.x < -0.1f)
    {
        if (currentDirection != Direction::Left)
        {
            currentDirection = Direction::Left;
            // Flip sprite horizontally
            setScale({-1.0f, 1.0f});
            // Adjust position since flipping changes the origin
            setOrigin({128.0f, 0.0f});
        }
    }
    else if (vel.x > 0.1f)
    {
        if (currentDirection != Direction::Right)
        {
            currentDirection = Direction::Right;
            // Reset to normal
            setScale({1.0f, 1.0f});
            setOrigin({0.0f, 0.0f});
        }
    }
}

bool Player::isAnimationFinished() const
{
    auto it = animations.find(currentState);
    if (it == animations.end())
        return true;
    
    const AnimationData& anim = it->second;
    
    // Animation is finished if it's non-looping and we're on the last frame
    return !anim.loop && (frameId >= anim.frameCount - 1);
}

void Player::updateState()
{
    sf::Vector2f vel = getVelocity();
    
    // Update direction based on movement
    updateDirection();
    
    // Determine state based on velocity and ground contact
    if (!onGround)
    {
        if (vel.y < 0)
        {
            // Moving upward - jumping
            changeState(PlayerState::Jumping);
        }
        else
        {
            // Moving downward - need to determine if we should play BeginFalling or Falling
            if (currentState == PlayerState::BeginFalling)
            {
                // Already in BeginFalling, check if animation finished
                if (isAnimationFinished())
                {
                    // Transition to continuous falling
                    changeState(PlayerState::Falling);
                }
                // Otherwise stay in BeginFalling until animation completes
            }
            else if (currentState == PlayerState::Jumping && isAnimationFinished())
            {
                // Jumped and now starting to fall
                changeState(PlayerState::BeginFalling);
            }
            else if (wasOnGround && currentState != PlayerState::Jumping)
            {
                // Just walked off a ledge - start BeginFalling
                changeState(PlayerState::BeginFalling);
            }
            else if (currentState != PlayerState::Jumping && currentState != PlayerState::BeginFalling)
            {
                // Default to falling if not jumping or beginning to fall
                changeState(PlayerState::Falling);
            }
        }
    }
    else
    {
        // On ground
        if (std::abs(vel.x) > 0.1f)
        {
            // Moving horizontally - walking
            changeState(PlayerState::Walking);
        }
        else
        {
            // Not moving - idle
            changeState(PlayerState::Idle);
        }
    }
    
    // Update previous ground state for next frame
    wasOnGround = onGround;
}

void Player::animate(const sf::Time &elapsed)
{
    // Check if current state has animation data
    if (animations.find(currentState) == animations.end())
        return;
    
    const AnimationData& anim = animations[currentState];
    
    timeout += elapsed.asSeconds();
    
    if (timeout >= 1.0f / fps)
    {
        if (anim.loop)
        {
            // Looping animation
            if (frameId >= anim.frameCount - 1)
            {
                frameId = 0;
            }
            else
            {
                frameId++;
            }
        }
        else
        {
            // Non-looping animation (like jumping or begin falling)
            if (frameId < anim.frameCount - 1)
            {
                frameId++;
            }
            // Stay on last frame if reached
        }
        
        timeout = 0;
    }
    
    // Set the texture rect for current frame
    if (frameId < anim.frames.size())
    {
        setTextureRect(anim.frames[frameId]);
    }
}

void Player::handleInput()
{
    sf::Vector2f vel = getVelocity();
    
    // Horizontal movement - only modify X velocity
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        vel.x = -MOVE_SPEED;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        vel.x = MOVE_SPEED;
    }
    else
    {
        // No horizontal input, let friction handle it
        vel.x = getVelocity().x;
    }
    
    // Jump - only modify Y velocity when jumping
    if (onGround && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)))
    {
        vel.y = JUMP_FORCE;
        onGround = false;
    }
    else
    {
        // Keep existing Y velocity (gravity/jumping)
        vel.y = getVelocity().y;
    }
    
    setVelocity(vel);
}

void Player::jump()
{
    if (!onGround) return;
    
    sf::Vector2f vel = getVelocity();
    vel.y = JUMP_FORCE;
    setVelocity(vel);
    onGround = false;
}

bool Player::isOnGround() const
{
    return onGround;
}

void Player::setOnGround(bool onGround_)
{
    onGround = onGround_;
}

int Player::getScore() const
{
    return score;
}

void Player::addScore(int points)
{
    score += points;
}

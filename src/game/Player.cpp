#include "game/Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
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
        frameId = 0;
        timeout = 0;
        
        // FIXED: Immediately set texture rect to first frame of new state
        // This prevents showing wrong frames during state transitions
        if (animations.find(currentState) != animations.end())
        {
            const AnimationData& anim = animations[currentState];
            if (!anim.frames.empty())
            {
                setTextureRect(anim.frames[0]);
            }
        }
    }
}

void Player::updateDirection()
{
    sf::Vector2f vel = getVelocity();
    
    if (vel.x < -0.1f)
    {
        if (currentDirection != Direction::Left)
        {
            currentDirection = Direction::Left;
            setScale({-1.0f, 1.0f});
            setOrigin({128.0f, 0.0f});
        }
    }
    else if (vel.x > 0.1f)
    {
        if (currentDirection != Direction::Right)
        {
            currentDirection = Direction::Right;
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
    return !anim.loop && (frameId >= anim.frameCount - 1);
}

void Player::shootHook(const sf::Vector2f& mousePos)
{
    if (hook.isAttached())
        return;
    
    sf::Vector2f playerCenter = getPosition() + sf::Vector2f(64, 64);
    sf::Vector2f direction = mousePos - playerCenter;
    
    hook.shoot(playerCenter, direction);
}

void Player::releaseHook()
{
    hook.release();
}

void Player::updateHook(const sf::Time& elapsed, const std::vector<std::shared_ptr<Platform>>& platforms)
{
    sf::Vector2f playerCenter = getPosition() + sf::Vector2f(64, 64);
    
    hook.update(elapsed, playerCenter);
    
    if (hook.getState() == HookState::Shooting)
    {
        for (const auto& platform : platforms)
        {
            if (hook.checkPlatformCollision(platform, playerCenter))
            {
                break;
            }
        }
    }
    
    if (hook.isAttached())
    {
        if (hook.shouldBreak(playerCenter))
        {
            hook.release();
        }
    }
}

void Player::applySwingPhysics(const sf::Time& elapsed)
{
    if (!hook.isAttached())
        return;
    
    sf::Vector2f playerCenter = getPosition() + sf::Vector2f(64, 64);
    sf::Vector2f attachPoint = hook.getAttachPoint();
    
    sf::Vector2f ropeVec = playerCenter - attachPoint;
    float ropeLength = std::sqrt(ropeVec.x * ropeVec.x + ropeVec.y * ropeVec.y);
    
    if (ropeLength < 0.1f)
        return;
    
    sf::Vector2f ropeDir = ropeVec / ropeLength;
    sf::Vector2f tangent(-ropeDir.y, ropeDir.x);
    
    float swingInput = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        swingInput = -1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        swingInput = 1.0f;
    }
    
    sf::Vector2f vel = getVelocity();
    vel += tangent * swingInput * SWING_ACCELERATION * elapsed.asSeconds();
    vel.y += 980.0f * elapsed.asSeconds();
    
    float radialVel = vel.x * ropeDir.x + vel.y * ropeDir.y;
    vel -= ropeDir * radialVel;
    
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (speed > MAX_SWING_SPEED)
    {
        vel = vel / speed * MAX_SWING_SPEED;
    }
    
    setVelocity(vel);
    
    playerCenter = getPosition() + sf::Vector2f(64, 64);
    ropeVec = playerCenter - attachPoint;
    float currentLength = std::sqrt(ropeVec.x * ropeVec.x + ropeVec.y * ropeVec.y);
    
    if (currentLength > Hook::MAX_ROPE_LENGTH)
    {
        sf::Vector2f constrainedPos = attachPoint + (ropeVec / currentLength) * Hook::MAX_ROPE_LENGTH;
        setPosition(constrainedPos - sf::Vector2f(64, 64));
    }
}

void Player::updateState()
{
    sf::Vector2f vel = getVelocity();
    
    updateDirection();
    
    if (hook.isAttached())
    {
        changeState(PlayerState::Hooked);
        wasOnGround = onGround;
        return;
    }
    
    if (!onGround)
    {
        if (vel.y < 0)
        {
            changeState(PlayerState::Jumping);
        }
        else
        {
            if (currentState == PlayerState::BeginFalling)
            {
                if (isAnimationFinished())
                {
                    changeState(PlayerState::Falling);
                }
            }
            else if (currentState == PlayerState::Jumping && isAnimationFinished())
            {
                changeState(PlayerState::BeginFalling);
            }
            else if (wasOnGround && currentState != PlayerState::Jumping)
            {
                changeState(PlayerState::BeginFalling);
            }
            else if (currentState != PlayerState::Jumping && currentState != PlayerState::BeginFalling)
            {
                changeState(PlayerState::Falling);
            }
        }
    }
    else
    {
        if (std::abs(vel.x) > 0.1f)
        {
            changeState(PlayerState::Walking);
        }
        else
        {
            changeState(PlayerState::Idle);
        }
    }
    
    wasOnGround = onGround;
}

void Player::animate(const sf::Time &elapsed)
{
    if (animations.find(currentState) == animations.end())
        return;
    
    const AnimationData& anim = animations[currentState];
    
    timeout += elapsed.asSeconds();
    
    if (timeout >= 1.0f / fps)
    {
        if (anim.loop)
        {
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
            if (frameId < anim.frameCount - 1)
            {
                frameId++;
            }
        }
        
        timeout = 0;
    }
    
    // FIXED: Ensure frameId is always within valid bounds
    if (frameId >= anim.frames.size())
    {
        frameId = 0;
    }
    
    if (frameId < anim.frames.size())
    {
        setTextureRect(anim.frames[frameId]);
    }
}

void Player::handleInput(const sf::RenderWindow& window)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (!hook.isAttached() && hook.getState() == HookState::Inactive)
        {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);
            shootHook(mouseWorldPos);
        }
    }
    
    if (hook.isAttached())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            releaseHook();
        }
    }
    
    sf::Vector2f vel = getVelocity();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        if (!hook.isAttached())
            vel.x = -MOVE_SPEED;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        if (!hook.isAttached())
            vel.x = MOVE_SPEED;
    }
    
    if (onGround && !hook.isAttached() &&
        (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)))
    {
        vel.y = JUMP_FORCE;
        onGround = false;
    }
    
    if (!hook.isAttached())
        setVelocity(vel);
}

void Player::jump()
{
    if (!onGround || hook.isAttached()) return;
    
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

void Player::forceState(PlayerState newState)
{
    currentState = newState;
    frameId = 0;
    timeout = 0;
    
    // Set the texture rect immediately
    if (animations.find(currentState) != animations.end())
    {
        const AnimationData& anim = animations[currentState];
        if (!anim.frames.empty())
        {
            setTextureRect(anim.frames[0]);
        }
    }
}

void Player::reset()
{
    // Reset velocity and state
    velocity = sf::Vector2f(0, 0);
    onGround = false;
    wasOnGround = false;
    
    // Reset hook
    hook.release();
    
    // Reset facing direction
    currentDirection = Direction::Right;
    setScale({1.0f, 1.0f});
    setOrigin({0.0f, 0.0f});
    
    // Force state to Idle with proper frame reset
    forceState(PlayerState::Idle);
}
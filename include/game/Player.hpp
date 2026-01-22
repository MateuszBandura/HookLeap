#pragma once
#include "game/Character.hpp"
#include "game/Hook.hpp"
#include <map>

enum class PlayerState
{
    Idle,
    Walking,
    Jumping,
    BeginFalling,
    Falling,
    Hooked
};

enum class Direction
{
    Left,
    Right
};

class Player : public Character
{
public:
    Player(const sf::Texture& texture);
    
    void handleInput(const sf::RenderWindow& window);
    void jump();
    void animate(const sf::Time &elapsed);
    void updateState();
    void updateHook(const sf::Time& elapsed, const std::vector<std::shared_ptr<Platform>>& platforms);
    
    bool isOnGround() const;
    void setOnGround(bool onGround_);
    
    int getScore() const;
    void addScore(int points);
    
    void shootHook(const sf::Vector2f& mousePos);
    void releaseHook();
    bool isHooked() const { return hook.isAttached(); }
    Hook& getHook() { return hook; }
    const Hook& getHook() const { return hook; }
    
    void applySwingPhysics(const sf::Time& elapsed);
    
    void setAnimationRow(PlayerState state, int row, int frameCount, bool shouldLoop = true);
    
    // Reset player to initial state
    void reset();
    
    // Force a state change (useful for resets)
    void forceState(PlayerState newState);
    
    static constexpr float MOVE_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = -500.0f;
    static constexpr float SWING_ACCELERATION = 400.0f;
    static constexpr float MAX_SWING_SPEED = 600.0f;
    
private:
    int score;
    bool onGround;
    bool wasOnGround;
    
    Hook hook;
    
    PlayerState currentState;
    Direction currentDirection;
    
    struct AnimationData
    {
        int row;
        int frameCount;
        bool loop;
        std::vector<sf::IntRect> frames;
    };
    
    std::map<PlayerState, AnimationData> animations;
    
    void changeState(PlayerState newState);
    void updateDirection();
    bool isAnimationFinished() const;
};
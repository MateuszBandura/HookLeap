#pragma once
#include "game/Character.hpp"
#include <map>

enum class PlayerState
{
    Idle,
    Walking,
    Jumping,
    BeginFalling,  // Plays once when starting to fall
    Falling        // Continuous falling loop
};

enum class Direction
{
    Left,
    Right
};

struct AnimationData
{
    int row;
    int frameCount;
    bool loop;
    std::vector<sf::IntRect> frames;
};

class Player : public Character
{
public:
    Player(const sf::Texture& texture);
    
    void handleInput();
    void jump();
    void animate(const sf::Time &elapsed);
    void updateState(); // Updates animation state based on velocity/ground
    
    bool isOnGround() const;
    void setOnGround(bool onGround_);
    
    int getScore() const;
    void addScore(int points);
    
    // Animation setup
    void setAnimationRow(PlayerState state, int row, int frameCount, bool shouldLoop = true);
    
    static constexpr float MOVE_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = -500.0f;
    
private:
    int score;
    bool onGround;
    bool wasOnGround; // Track previous ground state
    PlayerState currentState;
    Direction currentDirection;
    std::map<PlayerState, AnimationData> animations;
    
    void changeState(PlayerState newState);
    void updateDirection();
    bool isAnimationFinished() const; // Check if non-looping animation finished
};

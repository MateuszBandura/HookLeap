#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "game/Player.hpp"
#include "core/Physics.hpp"
#include "game/AnimatedSprite.hpp"
#include "game/Platform.hpp"
#include "game/Pickup.hpp"
#include "game/Coin.hpp"
#include "game/Checkpoint.hpp"
#include "game/WinPickup.hpp"

enum class GameState
{
    Menu,
    Playing,
    Paused,
    WinScreen
};

struct MapButton
{
    sf::RectangleShape shape;
    std::unique_ptr<sf::Text> text;
    std::string mapFile;
};

class MainWindow 
{
public:
    MainWindow(unsigned int width = 1200, unsigned int height = 800, const std::string& title = "HookLeap");
    void run();
    
private:
    sf::RenderWindow window;
    sf::View camera;
    sf::Clock clock;
    sf::Clock gameTimer;
    
    GameState currentState;
    
    // Game objects
    std::unique_ptr<Player> player;
    sf::Texture characterTexture;
    Physics physics;
    std::vector<std::shared_ptr<Platform>> platforms;
    std::vector<std::shared_ptr<Pickup>> pickups;
    
    // Textures
    sf::Texture platformTexture;
    sf::Texture groundTexture;
    sf::Texture obstacleTexture;
    sf::Texture coinTexture;
    sf::Texture checkpointTexture;
    sf::Texture winPickupTexture;
    sf::Texture backgroundTexture;
    
    // Background sprite
    std::unique_ptr<sf::Sprite> background;
    std::string currentTileset;
    
    // Game stats
    int score;
    int deaths;
    float currentTime;
    sf::Vector2f lastCheckpoint;
    
    // UI
    sf::Font font;
    std::unique_ptr<sf::Text> scoreText;
    std::unique_ptr<sf::Text> timeText;
    std::unique_ptr<sf::Text> winScoreText;
    std::unique_ptr<sf::Text> winTimeText;
    std::unique_ptr<sf::Text> winDeathsText;
    
    // Menu
    std::unique_ptr<sf::Text> logoText;
    std::vector<MapButton> mapButtons;
    sf::RectangleShape quitButton;
    std::unique_ptr<sf::Text> quitButtonText;
    
    // Win screen buttons
    sf::RectangleShape restartButton;
    std::unique_ptr<sf::Text> restartButtonText;
    sf::RectangleShape menuButton;
    std::unique_ptr<sf::Text> menuButtonText;
    sf::RectangleShape winQuitButton;
    std::unique_ptr<sf::Text> winQuitButtonText;
    
    void init();
    void handleEvents();
    void handleMenuEvents(const sf::Event& event);
    void handlePlayingEvents(const sf::Event& event);
    void handleWinScreenEvents(const sf::Event& event);
    
    void update(sf::Time& elapsed);
    void updateMenu(sf::Time& elapsed);
    void updatePlaying(sf::Time& elapsed);
    void updateWinScreen(sf::Time& elapsed);
    
    void render();
    void renderMenu();
    void renderPlaying();
    void renderWinScreen();
    
    void loadMap(const std::string& mapFile);
    void clearMap();
    void setupMenu();
    void setupWinScreen();
    void updateCamera();
    void updateUI();
    void drawHookRope(const sf::Vector2f& start, const sf::Vector2f& end);
    
    void respawnPlayer();
    void collectPickup(std::shared_ptr<Pickup> pickup);
    void triggerWinScreen();
    void restartLevel();
    void returnToMenu();
};

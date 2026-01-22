#include "core/MainWindow.hpp"
#include <array>
#include <fstream>
#include <sstream>

MainWindow::MainWindow(unsigned int width, unsigned int height, const std::string& title)
    : currentState(GameState::Menu), score(0), deaths(0), currentTime(0.0f),
      lastCheckpoint(100, 250)
{
    window.create(sf::VideoMode({width, height}), title);
    
    camera.setSize({static_cast<float>(width), static_cast<float>(height)});
    camera.setCenter({static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f});
}

void MainWindow::clearMap()
{
    platforms.clear();
    pickups.clear();
    physics.clearPlatforms();
}

void MainWindow::loadMap(const std::string& mapFile)
{
    clearMap();
    
    std::ifstream file("assets/maps/" + mapFile);
    if (!file.is_open())
    {
        std::cerr << "Failed to open map file: " << mapFile << std::endl;
        return;
    }
    
    std::string line;
    
    // First line is tileset name
    std::getline(file, line);
    currentTileset = line;
    
    // Load tileset textures
    if (!groundTexture.loadFromFile("assets/" + currentTileset + "_ground.png"))
    {
        std::cerr << "Could not load ground texture for tileset: " << currentTileset << std::endl;
    }
    groundTexture.setRepeated(true);
    
    if (!platformTexture.loadFromFile("assets/" + currentTileset + "_platform.png"))
    {
        std::cerr << "Could not load platform texture for tileset: " << currentTileset << std::endl;
    }
    
    if (!obstacleTexture.loadFromFile("assets/" + currentTileset + "_obstacle.png"))
    {
        std::cerr << "Could not load obstacle texture for tileset: " << currentTileset << std::endl;
    }
    
    // Load background
    if (!backgroundTexture.loadFromFile("assets/" + currentTileset + ".png"))
    {
        std::cerr << "Could not load background for tileset: " << currentTileset << std::endl;
    }
    else
    {
        backgroundTexture.setRepeated(true);
        background = std::make_unique<sf::Sprite>(backgroundTexture);
        background->setPosition({-2000, -400});
        background->setTextureRect(sf::IntRect({0, 0}, {10000, 1600}));
        // Make background large enough to cover the map
        
    }
    
    // Parse map objects
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        
        if (type == "ground")
        {
            float x, y, width, height;
            iss >> x >> y >> width >> height;
            
            auto ground = std::make_shared<Platform>(groundTexture, PlatformType::Ground);
            ground->setPosition({x, y});
            ground->setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(width), static_cast<int>(height)}));
            platforms.push_back(ground);
            physics.addPlatform(ground);
        }
        else if (type == "platform")
        {
            float x, y;
            iss >> x >> y;
            
            auto platform = std::make_shared<Platform>(platformTexture, PlatformType::Floating);
            platform->setPosition({x, y});
            sf::Vector2f size = static_cast<sf::Vector2f>(platformTexture.getSize());
            platform->setSize(size.x, size.y);
            platforms.push_back(platform);
            physics.addPlatform(platform);
        }
        else if (type == "obstacle")
        {
            float x, y;
            iss >> x >> y;
            
            auto obstacle = std::make_shared<Platform>(obstacleTexture, PlatformType::DeathPit);
            obstacle->setPosition({x, y});
            sf::Vector2f size = static_cast<sf::Vector2f>(obstacleTexture.getSize());
            obstacle->setSize(size.x, size.y);
            platforms.push_back(obstacle);
            physics.addPlatform(obstacle);
        }
        else if (type == "pickup")
        {
            float x, y;
            iss >> x >> y;
            
            auto coin = std::make_shared<Coin>(coinTexture);
            coin->setPosition({x, y});
            pickups.push_back(coin);
        }
        else if (type == "checkpoint")
        {
            float x, y;
            iss >> x >> y;
            
            auto checkpoint = std::make_shared<Checkpoint>(checkpointTexture);
            checkpoint->setPosition({x, y});
            pickups.push_back(checkpoint);
        }
        else if (type == "win")
        {
            float x, y;
            iss >> x >> y;
            
            auto winPickup = std::make_shared<WinPickup>(winPickupTexture);
            winPickup->setPosition({x, y});
            pickups.push_back(winPickup);
        }
    }
    
    file.close();
    
    // Reset game state
    score = 0;
    deaths = 0;
    currentTime = 0.0f;
    lastCheckpoint = sf::Vector2f(100, 250);
    gameTimer.restart();
    
    // Reset player
    if (player)
    {
        player->setPosition(lastCheckpoint);
        player->reset();
        
        // IMPORTANT: Re-bind the character texture to ensure it's still correct
        // Loading other textures might affect sprite texture binding
        player->setTexture(characterTexture);
    }
    
    currentState = GameState::Playing;
}

void MainWindow::setupMenu()
{
    // Setup logo
    logoText = std::make_unique<sf::Text>(font);
    logoText->setString("HOOKLEAP");
    logoText->setCharacterSize(80);
    logoText->setFillColor(sf::Color::White);
    logoText->setPosition({window.getSize().x / 2.0f - 200, 100});
    
    // Setup map buttons
    mapButtons.clear();
    std::vector<std::string> maps = {"map1.txt", "map2.txt"};
    
    float startY = 300;
    float spacing = 80;
    
    for (size_t i = 0; i < maps.size(); ++i)
    {
        MapButton btn;
        btn.shape.setSize({300, 60});
        btn.shape.setPosition({window.getSize().x / 2.0f - 150, startY + i * spacing});
        btn.shape.setFillColor(sf::Color(100, 100, 100));
        
        btn.text = std::make_unique<sf::Text>(font);
        btn.text->setString("Level " + std::to_string(i + 1));
        btn.text->setCharacterSize(30);
        btn.text->setFillColor(sf::Color::White);
        btn.text->setPosition({btn.shape.getPosition().x + 80, btn.shape.getPosition().y + 15});
        
        btn.mapFile = maps[i];
        mapButtons.push_back(std::move(btn));
    }
    
    // Setup quit button
    quitButton.setSize({300, 60});
    quitButton.setPosition({window.getSize().x / 2.0f - 150, startY + maps.size() * spacing});
    quitButton.setFillColor(sf::Color(150, 50, 50));
    
    quitButtonText = std::make_unique<sf::Text>(font);
    quitButtonText->setString("Quit");
    quitButtonText->setCharacterSize(30);
    quitButtonText->setFillColor(sf::Color::White);
    quitButtonText->setPosition({quitButton.getPosition().x + 110, quitButton.getPosition().y + 15});
}

void MainWindow::setupWinScreen()
{
    // Win screen text
    winScoreText = std::make_unique<sf::Text>(font);
    winScoreText->setCharacterSize(40);
    winScoreText->setFillColor(sf::Color::White);
    winScoreText->setPosition({window.getSize().x / 2.0f - 100, 200});
    
    winTimeText = std::make_unique<sf::Text>(font);
    winTimeText->setCharacterSize(40);
    winTimeText->setFillColor(sf::Color::White);
    winTimeText->setPosition({window.getSize().x / 2.0f - 100, 260});
    
    winDeathsText = std::make_unique<sf::Text>(font);
    winDeathsText->setCharacterSize(40);
    winDeathsText->setFillColor(sf::Color::White);
    winDeathsText->setPosition({window.getSize().x / 2.0f - 100, 320});
    
    // Buttons
    float startY = 420;
    float spacing = 80;
    
    restartButton.setSize({300, 60});
    restartButton.setPosition({window.getSize().x / 2.0f - 150, startY});
    restartButton.setFillColor(sf::Color(100, 150, 100));
    
    restartButtonText = std::make_unique<sf::Text>(font);
    restartButtonText->setString("Restart");
    restartButtonText->setCharacterSize(30);
    restartButtonText->setFillColor(sf::Color::White);
    restartButtonText->setPosition({restartButton.getPosition().x + 80, restartButton.getPosition().y + 15});
    
    menuButton.setSize({300, 60});
    menuButton.setPosition({window.getSize().x / 2.0f - 150, startY + spacing});
    menuButton.setFillColor(sf::Color(100, 100, 150));
    
    menuButtonText = std::make_unique<sf::Text>(font);
    menuButtonText->setString("Main Menu");
    menuButtonText->setCharacterSize(30);
    menuButtonText->setFillColor(sf::Color::White);
    menuButtonText->setPosition({menuButton.getPosition().x + 60, menuButton.getPosition().y + 15});
    
    winQuitButton.setSize({300, 60});
    winQuitButton.setPosition({window.getSize().x / 2.0f - 150, startY + spacing * 2});
    winQuitButton.setFillColor(sf::Color(150, 50, 50));
    
    winQuitButtonText = std::make_unique<sf::Text>(font);
    winQuitButtonText->setString("Quit");
    winQuitButtonText->setCharacterSize(30);
    winQuitButtonText->setFillColor(sf::Color::White);
    winQuitButtonText->setPosition({winQuitButton.getPosition().x + 110, winQuitButton.getPosition().y + 15});
}

void MainWindow::init()
{
    // Load font
    auto fontResult = font.openFromFile("assets/font.otf");
    if (!fontResult)
    {
        std::cerr << "Could not load font, using default" << std::endl;
    }
    
    // Load character texture
    if(!characterTexture.loadFromFile("assets/hero.png"))
    {
        std::cerr << "Could not load texture" << std::endl;
    }
    
    // Load pickup textures
    if (!coinTexture.loadFromFile("assets/coin.png"))
    {
        std::cerr << "Could not load coin texture" << std::endl;
    }
    
    if (!checkpointTexture.loadFromFile("assets/checkpoint.png"))
    {
        std::cerr << "Could not load checkpoint texture" << std::endl;
    }
    
    if (!winPickupTexture.loadFromFile("assets/win.png"))
    {
        std::cerr << "Could not load win pickup texture" << std::endl;
    }

    player = std::make_unique<Player>(characterTexture);
    
    // Setup animations
    player->setAnimationRow(PlayerState::Idle, 1, 10, true);
    player->setAnimationRow(PlayerState::Walking, 3, 10, true);
    player->setAnimationRow(PlayerState::Jumping, 10, 6, false);
    player->setAnimationRow(PlayerState::BeginFalling, 11, 4, false);
    player->setAnimationRow(PlayerState::Falling, 12, 3, true); 
    player->setAnimationRow(PlayerState::Hooked, 13, 4, true);
    
    player->setFps(20);
    player->setPosition({100, 250});
    player->setHitbox(54, 44, 20, 37);
    
    // Setup UI
    scoreText = std::make_unique<sf::Text>(font);
    scoreText->setCharacterSize(30);
    scoreText->setFillColor(sf::Color::White);
    
    timeText = std::make_unique<sf::Text>(font);
    timeText->setCharacterSize(30);
    timeText->setFillColor(sf::Color::White);
    
    setupMenu();
    setupWinScreen();
}

void MainWindow::handleMenuEvents(const sf::Event& event)
{
    if (event.is<sf::Event::MouseButtonPressed>())
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        // Check map buttons
        for (const auto& btn : mapButtons)
        {
            if (btn.shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                loadMap(btn.mapFile);
                return;
            }
        }
        
        // Check quit button
        if (quitButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
        {
            window.close();
        }
    }
}

void MainWindow::handlePlayingEvents(const sf::Event& event)
{
    if (event.is<sf::Event::KeyPressed>())
    {
        const auto& keyEvent = *event.getIf<sf::Event::KeyPressed>();
        if (keyEvent.code == sf::Keyboard::Key::Escape)
        {
            currentState = GameState::Paused;
        }
    }
}

void MainWindow::handleWinScreenEvents(const sf::Event& event)
{
    if (event.is<sf::Event::MouseButtonPressed>())
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (restartButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
        {
            restartLevel();
        }
        else if (menuButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
        {
            returnToMenu();
        }
        else if (winQuitButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
        {
            window.close();
        }
    }
}

void MainWindow::handleEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
            
        switch (currentState)
        {
            case GameState::Menu:
                handleMenuEvents(*event);
                break;
            case GameState::Playing:
                handlePlayingEvents(*event);
                break;
            case GameState::WinScreen:
                handleWinScreenEvents(*event);
                break;
            case GameState::Paused:
                if (event->is<sf::Event::KeyPressed>())
                {
                    currentState = GameState::Playing;
                }
                break;
        }
    }
}

void MainWindow::updateCamera()
{
    if (!player->isAlive())
        return;
    
    sf::FloatRect playerHitbox = player->getGlobalHitbox();
    
    sf::Vector2f hitboxCenter(
        playerHitbox.position.x + playerHitbox.size.x / 2.0f,
        playerHitbox.position.y + playerHitbox.size.y / 2.0f
    );
    
    sf::Vector2f currentCenter = camera.getCenter();
    float smoothing = 0.1f;
    sf::Vector2f newCenter = currentCenter + (hitboxCenter - currentCenter) * smoothing;
    
    camera.setCenter(newCenter);
}

void MainWindow::updateUI()
{
    // Update score text
    scoreText->setString("Score: " + std::to_string(score));
    sf::Vector2f topLeft = camera.getCenter() - camera.getSize() / 2.0f;
    scoreText->setPosition({topLeft.x + 20, topLeft.y + 20});
    
    // Update time text
    int minutes = static_cast<int>(currentTime) / 60;
    int seconds = static_cast<int>(currentTime) % 60;
    int milliseconds = static_cast<int>((currentTime - static_cast<int>(currentTime)) * 100);
    
    std::ostringstream timeStream;
    timeStream << minutes << ":" 
               << (seconds < 10 ? "0" : "") << seconds << "."
               << (milliseconds < 10 ? "0" : "") << milliseconds;
    
    timeText->setString(timeStream.str());
    sf::Vector2f topRight = camera.getCenter() + sf::Vector2f(camera.getSize().x / 2.0f, -camera.getSize().y / 2.0f);
    sf::FloatRect timeBounds = timeText->getLocalBounds();
    timeText->setPosition({topRight.x - timeBounds.size.x - 20, topRight.y + 20});
}

void MainWindow::respawnPlayer()
{
    deaths++;
    player->setPosition({lastCheckpoint.x-16, lastCheckpoint.y-16});
    player->reset();
}

void MainWindow::collectPickup(std::shared_ptr<Pickup> pickup)
{
    if (pickup->isCollected())
        return;
    
    pickup->collect();
    
    // Check pickup type and handle accordingly
    if (auto coin = std::dynamic_pointer_cast<Coin>(pickup))
    {
        score++;
    }
    else if (auto checkpoint = std::dynamic_pointer_cast<Checkpoint>(pickup))
    {
        lastCheckpoint = checkpoint->getPosition();
    }
    else if (auto winPickup = std::dynamic_pointer_cast<WinPickup>(pickup))
    {
        triggerWinScreen();
    }
}

void MainWindow::triggerWinScreen()
{
    currentState = GameState::WinScreen;
    
    // Update win screen text
    winScoreText->setString("Score: " + std::to_string(score));
    
    int minutes = static_cast<int>(currentTime) / 60;
    int seconds = static_cast<int>(currentTime) % 60;
    int milliseconds = static_cast<int>((currentTime - static_cast<int>(currentTime)) * 100);
    
    std::ostringstream timeStream;
    timeStream << "Time: " << minutes << ":" 
               << (seconds < 10 ? "0" : "") << seconds << "."
               << (milliseconds < 10 ? "0" : "") << milliseconds;
    winTimeText->setString(timeStream.str());
    
    winDeathsText->setString("Deaths: " + std::to_string(deaths));
}

void MainWindow::restartLevel()
{
    // Reload current map
    std::string currentMap;
    for (const auto& btn : mapButtons)
    {
        // Find which map was loaded (you might want to track this)
        currentMap = btn.mapFile;
        break;
    }
    
    if (!currentMap.empty())
    {
        loadMap(currentMap);
    }
}

void MainWindow::returnToMenu()
{
    clearMap();
    currentState = GameState::Menu;
    camera.setCenter({static_cast<float>(window.getSize().x) / 2.0f, 
                      static_cast<float>(window.getSize().y) / 2.0f});
}

void MainWindow::updateMenu(sf::Time& elapsed)
{
    // Menu doesn't need updates
}

void MainWindow::updatePlaying(sf::Time& elapsed)
{
    if (!player->isAlive())
        return;
    
    // Update game timer
    currentTime = gameTimer.getElapsedTime().asSeconds();
    
    // Handle input
    player->handleInput(window);
    
    // Update hook
    player->updateHook(elapsed, platforms);
    
    // Apply physics differently based on hook state
    if (player->isHooked())
    {
        player->applySwingPhysics(elapsed);
        player->moveCharacter(elapsed);
        
        sf::Vector2f velocity = player->getVelocity();
        bool fellInPit = false;
        bool hitDeadlyPlatform = false;
        bool onGround = physics.handleCollisions(*player, velocity, fellInPit, hitDeadlyPlatform);
        player->setOnGround(onGround);
        player->setVelocity(velocity);
        
        if (onGround)
        {
            player->releaseHook();
        }
    }
    else
    {
        sf::Vector2f velocity = player->getVelocity();
        physics.applyGravity(velocity, elapsed);
        player->setVelocity(velocity);
        
        player->moveCharacter(elapsed);
        
        bool fellInPit = false;
        bool hitDeadlyPlatform = false;
        bool onGround = physics.handleCollisions(*player, velocity, fellInPit, hitDeadlyPlatform);
        player->setOnGround(onGround);
        player->setVelocity(velocity);
        
        velocity = player->getVelocity();
        physics.applyFriction(velocity, onGround);
        player->setVelocity(velocity);
        
        if (fellInPit || hitDeadlyPlatform)
        {
            respawnPlayer();
        }
    }
    
    // Check pickup collisions
    sf::FloatRect playerBounds = player->getGlobalHitbox();
    for (auto& pickup : pickups)
    {
        if (!pickup->isCollected())
        {
            sf::FloatRect pickupBounds = pickup->getGlobalBounds();
            if (playerBounds.findIntersection(pickupBounds))
            {
                collectPickup(pickup);
            }
        }
    }
    
    // Update pickups
    for (auto& pickup : pickups)
    {
        pickup->animate(elapsed);
    }
    
    // Update player
    player->updateState();
    player->animate(elapsed);
    
    updateCamera();
    updateUI();
}

void MainWindow::updateWinScreen(sf::Time& elapsed)
{
    // Win screen doesn't need updates
}

void MainWindow::update(sf::Time& elapsed)
{
    switch (currentState)
    {
        case GameState::Menu:
            updateMenu(elapsed);
            break;
        case GameState::Playing:
            updatePlaying(elapsed);
            break;
        case GameState::WinScreen:
            updateWinScreen(elapsed);
            break;
        case GameState::Paused:
            // No updates when paused
            break;
    }
}

void MainWindow::drawHookRope(const sf::Vector2f& start, const sf::Vector2f& end)
{
    std::array<sf::Vertex, 2> line;
    line[0].position = start;
    line[0].color = sf::Color(100, 100, 100);
    line[1].position = end;
    line[1].color = sf::Color(100, 100, 100);
    
    window.draw(line.data(), 2, sf::PrimitiveType::Lines);
}

void MainWindow::renderMenu()
{
    window.clear(sf::Color(50, 50, 50));
    
    // Use default view for menu
    window.setView(window.getDefaultView());
    
    if (logoText)
        window.draw(*logoText);
    
    for (const auto& btn : mapButtons)
    {
        window.draw(btn.shape);
        if (btn.text)
        {
            window.draw(*btn.text);
        }
    }
    
    window.draw(quitButton);
    if (quitButtonText)
        window.draw(*quitButtonText);
}

void MainWindow::renderPlaying()
{
    window.clear(sf::Color(135, 206, 235));
    
    window.setView(camera);
    
    // Draw background (moves with camera)
    if (background)
    {
        //background->setPosition({camera.getCenter().x-window.getSize().x/2, camera.getCenter().y-window.getSize().y/2});
        window.draw(*background);
    }
    
    // Draw platforms
    for (const auto& platform : platforms)
    {
        window.draw(*platform);
    }
    
    // Draw pickups
    for (const auto& pickup : pickups)
    {
        if (!pickup->isCollected() || pickup->shouldRemainVisible())
        {
            window.draw(*pickup);
        }
    }
    
    // Draw hook rope if attached
    if (player->isHooked())
    {
        sf::Vector2f playerCenter = player->getPosition() + sf::Vector2f(64, 64);
        sf::Vector2f attachPoint = player->getHook().getAttachPoint();
        drawHookRope(playerCenter, attachPoint);
    }
    
    // Draw hook projectile
    player->getHook().draw(window);
    
    // Draw player
    window.draw(*player);
    
    // Draw UI
    if (scoreText)
        window.draw(*scoreText);
    if (timeText)
        window.draw(*timeText);
}

void MainWindow::renderWinScreen()
{
    window.clear(sf::Color(30, 30, 50));
    
    window.setView(window.getDefaultView());
    
    // Draw win text
    sf::Text winTitle(font);
    winTitle.setString("LEVEL COMPLETE!");
    winTitle.setCharacterSize(60);
    winTitle.setFillColor(sf::Color::Yellow);
    winTitle.setPosition({window.getSize().x / 2.0f - 250, 100});
    window.draw(winTitle);
    
    if (winScoreText)
        window.draw(*winScoreText);
    if (winTimeText)
        window.draw(*winTimeText);
    if (winDeathsText)
        window.draw(*winDeathsText);
    
    window.draw(restartButton);
    if (restartButtonText)
        window.draw(*restartButtonText);
    window.draw(menuButton);
    if (menuButtonText)
        window.draw(*menuButtonText);
    window.draw(winQuitButton);
    if (winQuitButtonText)
        window.draw(*winQuitButtonText);
}

void MainWindow::render()
{
    switch (currentState)
    {
        case GameState::Menu:
            renderMenu();
            break;
        case GameState::Playing:
            renderPlaying();
            break;
        case GameState::WinScreen:
            renderWinScreen();
            break;
        case GameState::Paused:
            renderPlaying(); // Still show game when paused
            // TODO: Add pause overlay
            break;
    }
    
    window.display();
}

void MainWindow::run()
{
    init();

    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        handleEvents();
        update(elapsed);
        render();
    }
}
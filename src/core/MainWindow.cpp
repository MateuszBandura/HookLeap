#include "core/MainWindow.hpp"

MainWindow::MainWindow(unsigned int width, unsigned int height, const std::string& title)
{
    window.create(sf::VideoMode({width, height}), title);
    
    camera.setSize({static_cast<float>(width), static_cast<float>(height)});
    camera.setCenter({static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f});
}

void MainWindow::setupMap()
{
    // Ground
    auto ground = std::make_shared<Platform>(platformTexture, PlatformType::Ground);
    ground->setTextureRect(sf::IntRect({0, 0}, {320, 16}));
    ground->setPosition({0, 500});
    platforms.push_back(ground);
    physics.addPlatform(ground);
    
    // Floating platforms
    auto platform1 = std::make_shared<Platform>(platformTexture, PlatformType::Floating);
    platform1->setPosition({200, 400});
    platform1->setSize(150, 20);
    platforms.push_back(platform1);
    physics.addPlatform(platform1);
    
    auto platform2 = std::make_shared<Platform>(platformTexture, PlatformType::Floating);
    platform2->setPosition({450, 350});
    platform2->setSize(150, 20);
    platforms.push_back(platform2);
    physics.addPlatform(platform2);
    
    auto platform3 = std::make_shared<Platform>(platformTexture, PlatformType::Floating);
    platform3->setPosition({100, 250});
    platform3->setSize(120, 20);
    platforms.push_back(platform3);
    physics.addPlatform(platform3);
    
    auto platform4 = std::make_shared<Platform>(platformTexture, PlatformType::Floating);
    platform4->setPosition({550, 200});
    platform4->setSize(120, 20);
    platforms.push_back(platform4);
    physics.addPlatform(platform4);
    
    auto platform5 = std::make_shared<Platform>(platformTexture, PlatformType::Floating);
    platform5->setPosition({900, 300});
    platform5->setSize(150, 20);
    platforms.push_back(platform5);
    physics.addPlatform(platform5);
    
    auto platform6 = std::make_shared<Platform>(platformTexture, PlatformType::Floating);
    platform6->setPosition({1200, 250});
    platform6->setSize(150, 20);
    platforms.push_back(platform6);
    physics.addPlatform(platform6);
}

void MainWindow::init()
{
    if(!characterTexture.loadFromFile("assets/hero.png"))
    {
        std::cerr << "Could not load texture" << std::endl;
    }
    
    // Load platform texture
    if(!platformTexture.loadFromFile("assets/ground.png"))
    {
        std::cerr << "Could not load platform texture, creating default" << std::endl;
        sf::Image img;
        img.resize({100, 100});
        img.setPixel({0, 0}, sf::Color(100, 100, 100));
        for (unsigned int y = 0; y < 100; ++y)
        {
            for (unsigned int x = 0; x < 100; ++x)
            {
                img.setPixel({x, y}, sf::Color(100, 100, 100));
            }
        }
        platformTexture.loadFromImage(img);
    }
    platformTexture.setRepeated(true);

    player = std::make_unique<Player>(characterTexture);
    
    player->setAnimationRow(PlayerState::Idle, 1, 10, true);
    player->setAnimationRow(PlayerState::Walking, 3, 10, true);
    player->setAnimationRow(PlayerState::Jumping, 10, 6, false);
    player->setAnimationRow(PlayerState::BeginFalling, 11, 4, false);
    player->setAnimationRow(PlayerState::Falling, 12, 3, true);
    
    player->setFps(20); // Animation speed
    
    // Set starting position
    player->setPosition({100, 250});
    
    // Set custom hitbox
    player->setHitbox(54, 44, 20, 37);
    
    // Setup the map
    setupMap();
}

void MainWindow::handleEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
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

void MainWindow::update(sf::Time& elapsed)
{
    if (!player->isAlive())
        return;
    
    // Handle input
    player->handleInput();
    
    // Apply physics
    sf::Vector2f velocity = player->getVelocity();
    physics.applyGravity(velocity, elapsed);
    player->setVelocity(velocity);
    
    // Move character
    player->moveCharacter(elapsed);
    
    // Handle collisions
    bool fellInPit = false;
    bool hitDeadlyPlatform = false;
    bool onGround = physics.handleCollisions(*player, velocity, fellInPit, hitDeadlyPlatform);
    player->setOnGround(onGround);
    player->setVelocity(velocity);
    
    // Apply friction
    velocity = player->getVelocity();
    physics.applyFriction(velocity, onGround);
    player->setVelocity(velocity);
    
    // Check for death
    if (fellInPit)
    {
        player->damage(player->getHealth());
        std::cout << "Player fell into a pit!" << std::endl;
    }
    
    if (hitDeadlyPlatform)
    {
        player->damage(player->getHealth());
        std::cout << "Player hit deadly platform!" << std::endl;
    }
    
    // Update animation state based on velocity/ground
    player->updateState();
    
    // Update animation
    player->animate(elapsed);
    
    // Update camera to follow player
    updateCamera();
}

void MainWindow::render()
{
    window.clear(sf::Color(135, 206, 235));
    
    window.setView(camera);
    
    for (const auto& platform : platforms)
    {
        window.draw(*platform);
    }
    
    window.draw(*player);
    
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

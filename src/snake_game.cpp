#include <chrono>
#include <SFML/Window.hpp>

void update_game(uint32_t) {}
void render_game_frame() {}

int main()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    const sf::Time TimePerFrame = sf::seconds(1.f/60.f);
    sf::Window window(sf::VideoMode(800, 600), "snake");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    constexpr uint32_t time_step_in_ms = 16;
    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;
        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            update_game(TimePerFrame.asMilliseconds());
        }

        // render game world
        render_game_frame();

        // handle user actions
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}

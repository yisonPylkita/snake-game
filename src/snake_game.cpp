#include <array>
#include <chrono>
#include <memory>
#include <vector>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace {
namespace resources {
namespace textures {
constexpr uint32_t snake_segment = 0x01u;
}
}

struct ResourceManager
{
    void init()
    {
        load_texture("./assets/snake_segment.bmp", resources::textures::snake_segment);
    }

    const sf::Texture & texture(uint32_t resource_id) const
    {
        if (_textures.find(resource_id) == _textures.end())
            throw std::invalid_argument("Texture '" + std::to_string(resource_id) + "' was not loaded");
        return _textures.at(resource_id);
    }

private:
    void load_texture(const std::string &file_path, uint32_t id)
    {
        sf::Texture texture;
        if (!texture.loadFromFile(file_path))
            throw std::runtime_error("Texture '" + file_path + "' not found");
        _textures[id] = texture;
    }

private:
    std::map<uint32_t, sf::Texture> _textures;
};

ResourceManager resource_manager;

struct SnakeSegment : public sf::Drawable
{
    uint32_t pos_x;
    uint32_t pos_y;

private :
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        sf::Sprite segment_sprite;
        segment_sprite.setPosition(pos_x, pos_y);
        segment_sprite.setTexture(resource_manager.texture(resources::textures::snake_segment));
        target.draw(segment_sprite, states);
    }
};

struct Snake : public sf::Drawable
{
protected:
    const std::vector<SnakeSegment> & get_segments() const
    {
        return _segments;
    }

private :
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        for (const auto &segment : _segments)
            target.draw(segment, states);
    }

private:
    std::vector<SnakeSegment> _segments;
};

constexpr uint32_t board_width = 64;
constexpr uint32_t board_height = 64;
std::array<std::array<uint8_t, board_width>, board_height> board{};

void update_game(uint32_t) {}
void render_game_frame() {}

int main_impl()
{
    resource_manager.init();

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

    return EXIT_SUCCESS;
}
}

int main()
{
    int exit_code = EXIT_FAILURE;
    try {
        exit_code = main_impl();
    } catch (const std::exception &ex) {
        std::cout << "Unhandled exception " - ex.what() << std::endl;
    } catch (...) {
        std::cout << "Unhandled unrecognized exception" << std::endl;
    }

    return exit_code;
}

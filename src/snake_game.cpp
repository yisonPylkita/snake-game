#include <array>
#include <chrono>
#include <memory>
#include <vector>
#include <deque>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace {
namespace resources {
namespace textures {
constexpr uint32_t snake_segment_body = 0x01u;
// constexpr uint32_t snake_segment_head = 0x02u;
}
}

struct ResourceManager
{
    void init()
    {
        load_texture("./assets/snake_segment_body.bmp", resources::textures::snake_segment_body);
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
    static constexpr uint32_t size_x = 50;
    static constexpr uint32_t size_y = 50;

    uint32_t pos_x;
    uint32_t pos_y;

private :
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        sf::Sprite segment_sprite;
        segment_sprite.setPosition(pos_x, pos_y);
        segment_sprite.setTexture(resource_manager.texture(resources::textures::snake_segment_body));
        target.draw(segment_sprite, states);
    }
};

struct Snake : public sf::Drawable
{
public:
    void init() {
        // Snake should start with a one segment
        auto segment = SnakeSegment();
        segment.pos_x = 600;
        segment.pos_y = 300;
        _segments.push_back(segment);
    }

    void set_direction(sf::Keyboard::Key direction) {
        if (direction == sf::Keyboard::Up || direction == sf::Keyboard::Down ||
            direction == sf::Keyboard::Left || direction == sf::Keyboard::Right) {
                _direction = direction;
        }
    }

    void take_step() {
        using sf::Keyboard;
        auto new_snake_head_segment = _segments.front();

        // TODO: implement movenet here
        if (_direction == Keyboard::Up) {
            new_snake_head_segment.pos_y -= SnakeSegment::size_y; 
        } else if (_direction == Keyboard::Down) {
            new_snake_head_segment.pos_y += SnakeSegment::size_y; 
        } else if (_direction == Keyboard::Left) {
            new_snake_head_segment.pos_x -= SnakeSegment::size_x; 
        } else if (_direction == Keyboard::Right) {
            new_snake_head_segment.pos_x += SnakeSegment::size_x; 
        }

        _segments.pop_back();
        _segments.push_front(new_snake_head_segment);
    }

protected:
    const std::deque<SnakeSegment> & get_segments() const
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
    sf::Keyboard::Key _direction = sf::Keyboard::Up;
    std::deque<SnakeSegment> _segments;
};

struct Candy : public sf::Drawable
{
    void init() {
        _rect.setSize(sf::Vector2f(50.f, 50.f));
        _rect.setPosition(_position);
        _rect.setFillColor(sf::Color::Red);
    }
private :
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        target.draw(_rect);
    }

private:
    sf::Vector2f _position = sf::Vector2f(50.f, 50.f);
    sf::RectangleShape _rect;
};

Candy candy;
Snake snake{};

void handle_key(sf::Event::KeyEvent key) {
    snake.set_direction(key.code);
}

void update_game() {
    snake.take_step();
}

void render_game_frame(sf::RenderWindow &window) {
    window.clear(sf::Color::White);

    window.draw(candy);
    window.draw(snake);

    window.display();
}

int main_impl()
{
    resource_manager.init();
    snake.init();
    candy.init();

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    const sf::Time TimePerFrame = sf::seconds(1.f/60.f);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Snake - the game");
    window.setFramerateLimit(10);
    // window.setVerticalSyncEnabled(true);
    constexpr uint32_t time_step_in_ms = 16;
    while (window.isOpen()) {
        // sf::Time dt = clock.restart();
        // timeSinceLastUpdate += dt;
        // while (timeSinceLastUpdate > TimePerFrame) {
        //     timeSinceLastUpdate -= TimePerFrame;
            // update_game(TimePerFrame.asMilliseconds());
            update_game();
        // }

        // render game world
        render_game_frame(window);

        // handle user actions
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                handle_key(event.key);
            }
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

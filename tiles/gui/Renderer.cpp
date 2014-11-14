
#include "Renderer.hpp"

#include <cassert>

sf::Color getColor(int color) {
    switch (color) {
        default: return sf::Color(0, 0, 0);
        case 0: return sf::Color(255, 0, 0);
        case 1: return sf::Color(0, 255, 0);
        case 2: return sf::Color(0, 0, 255);
    }
}

sf::Color darken(const sf::Color& c) {
    return sf::Color(
        std::max(0, c.r - 100),
        std::max(0, c.g - 100),
        std::max(0, c.b - 100)
    );
}

Renderer::Renderer() :
    tiles_original(read_from()),
    tiles_result(read_from()),
    window(sf::VideoMode(640, 640), "Tiles GUI") {}


void Renderer::run() {
    while (window.isOpen()) {
        handleEvents();
        draw();
    }
}

void Renderer::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                handleMouseButtonPressedEvent(event.mouseButton);
                break;
            default:
                break;
        }
    }
}

void Renderer::handleMouseButtonPressedEvent(
        const sf::Event::MouseButtonEvent& ev)
{
    unsigned columns = tiles_original.shape()[0];
    unsigned rows = tiles_original.shape()[1];

    unsigned width = window.getSize().x;
    unsigned height = window.getSize().y;

    if (ev.x < 0 || ev.x >= int(width) ||
        ev.y < 0 || ev.y >= int(height)) return;

    unsigned x = ev.x / (width / float(columns));
    unsigned y = ev.y / (height / float(rows));

    if (marked && *marked == position_t{x, y}) {
        marked = boost::none;
    } else {
        marked = position_t{x, y};
    }
}

void Renderer::draw() {
    window.clear();
    unsigned rows = tiles_original.shape()[1];
    unsigned columns = tiles_original.shape()[0];

    float width = window.getSize().x;
    float height = window.getSize().y;

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            sf::RectangleShape rect(
                    sf::Vector2f(width/columns, height/rows));
            rect.setPosition(x * width/columns, y * height/rows);
            rect.setFillColor(getColor(tiles_original[x][y]));
            rect.setOutlineColor(sf::Color::Black);
            rect.setOutlineThickness(1.f);

            if (marked && *marked == position_t{x, y}) {
                rect.setFillColor(darken(rect.getFillColor()));
            }
            window.draw(rect);
        }
    }

    window.display();
}


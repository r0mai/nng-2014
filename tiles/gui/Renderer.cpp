
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
        if (event.type == sf::Event::Closed) {
            window.close();
        }
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
            window.draw(rect);
        }
    }

    window.display();
}


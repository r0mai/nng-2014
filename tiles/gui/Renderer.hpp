
#ifndef RENDERED_HPP_
#define RENDERED_HPP_

#include "../common.hpp"
#include <SFML/Graphics.hpp>

sf::Color getColor(int color);

struct Renderer {

    Renderer();

    void run();
    void handleEvents();
    void draw();

    tiles_t tiles_original;
    tiles_t tiles_result;
    sf::RenderWindow window;
};

#endif

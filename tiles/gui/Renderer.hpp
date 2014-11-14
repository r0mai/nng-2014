
#ifndef RENDERED_HPP_
#define RENDERED_HPP_

#include "../common.hpp"
#include <SFML/Graphics.hpp>

#include <boost/optional.hpp>

sf::Color getColor(int color);
sf::Color darken(const sf::Color& c);

struct Renderer {

    Renderer();

    void run();
    void handleEvents();
    void handleMouseButtonPressedEvent(const sf::Event::MouseButtonEvent& ev);
    void draw();

    tiles_t tiles_original;
    tiles_t tiles_result;

    boost::optional<position_t> marked;

    sf::RenderWindow window;
};

#endif

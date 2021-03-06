
#include "Renderer.hpp"

#include <cassert>
#include <sstream>
#include <algorithm>
#include <fstream>

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
    window(sf::VideoMode(768, 768),
            get_title(is_done(tiles_result),
                get_swaps(tiles_original, tiles_result).size())) {}


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
    } else if (marked) {
        do_swap(*marked, position_t{x, y});
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

    const float outline_width = width/columns * 0.25;

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            sf::RectangleShape rect(
                    sf::Vector2f(width/columns, height/rows));

            sf::RectangleShape rect_in(
                    sf::Vector2f(
                        width/columns - 2*outline_width,
                        height/rows - 2*outline_width));

            rect.setPosition(x * width/columns, y * height/rows);
            rect.setFillColor(getColor(tiles_result[x][y]));

            rect_in.setPosition(
                    x * width/columns + outline_width,
                    y * height/rows + outline_width);
            rect_in.setFillColor(getColor(tiles_original[x][y]));

            if (marked && *marked == position_t{x, y}) {
                rect.setFillColor(darken(rect.getFillColor()));
                rect_in.setFillColor(darken(rect_in.getFillColor()));
            }
            window.draw(rect);
            window.draw(rect_in);
        }
    }

    window.display();
}

void Renderer::do_swap(const position_t& lhs, const position_t& rhs) {
    std::swap(tiles_result[lhs.x][lhs.y], tiles_result[rhs.x][rhs.y]);

    auto islands = get_islands(tiles_result);
    bool done = islands.size() == 3;
    swaps_t swaps = get_swaps(tiles_original, tiles_result);
    window.setTitle(get_title(done, swaps.size()));

    if (done) {
        if (!min_score || *min_score > swaps.size()) {
            min_score = swaps.size();
            std::ofstream out("tiles_manual_" + std::to_string(*min_score) + ".out");
            std::ofstream out_matrix("tiles_manual_" + std::to_string(*min_score) + ".out.matrix");
            print_swaps(swaps, out);
            print_tiles_as_input(tiles_result, out_matrix);
        }
    } else {
        std::cout << "islands.size() = " << islands.size() << std::endl;
    }
}

std::string Renderer::get_title(bool done, unsigned score) {
    std::stringstream ss;
    ss << std::boolalpha << "done = " << done
       << ", score = " << score;
    return ss.str();
}

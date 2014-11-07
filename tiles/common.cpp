
#include <iostream>
#include <tuple>
#include <cstdlib>

#include "common.hpp"

tiles_t read_from_cin() {
    unsigned rows, columns;

    std::cin >> rows >> columns;

    tiles_t tiles(boost::extents[columns][rows]);

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            std::cin >> tiles[x][y];
        }
    }

    return tiles;
}

vertex_property& get_vertex_property(vertex_descriptor vertex, graph_t& graph) {
    return boost::get(vertex_property_tag(), graph, vertex);
}

edge_property& get_edge_property(edge_descriptor edge, graph_t& graph) {
    return boost::get(edge_property_tag(), graph, edge);
}

graph_t create_graph(const tiles_t& tiles) {
    typedef std::vector<std::vector<vertex_descriptor>> vertex_matrix_t;

    unsigned rows = tiles.shape()[1];
    unsigned columns = tiles.shape()[0];

    graph_t graph;
    vertex_matrix_t vm(columns, std::vector<vertex_descriptor>(rows));

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            vm[x][y] = boost::add_vertex(graph);
            get_vertex_property(vm[x][y], graph) = {x, y, tiles[x][y]};
        }
    }

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            if (y > 0)         boost::add_edge(vm[x][y], vm[x][y-1], graph);
            if (y < rows-1)    boost::add_edge(vm[x][y], vm[x][y+1], graph);
            if (x > 0)         boost::add_edge(vm[x][y], vm[x-1][y], graph);
            if (x < columns-1) boost::add_edge(vm[x][y], vm[x+1][y], graph);
        }
    }
    return graph;
}

graph_t get_color_graph(graph_t graph, int color) {
    BOOST_FOREACH(vertex_descriptor vertex, boost::vertices(graph)) {
        if (get_vertex_property(vertex, graph).color != color) {
            clear_vertex(vertex, graph);
            remove_vertex(vertex, graph);
        }
    }
    return graph;
}

bool is_done(const tiles_t& tiles) {
    //TODO optimize
    return get_islands(tiles).size() == 3;
}


islands_t get_islands(const tiles_t& tiles) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    island_map_t island_map(boost::extents[columns][rows]);
    islands_t islands;

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            island_map[x][y] = -1;
        }
    }

    int island_counter = 0;
    // I think this traversing order is better for the cache
    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            if (island_map[x][y] == -1) {
                islands.push_back(
                    island_t(
                        tiles[x][y],
                        island_counter,
                        flood_and_paint(tiles, position_t{x, y}, island_map, island_counter)
                    )
                );
                ++island_counter;
            }
        }
    }

    return islands;
}

positions_t flood_and_paint(const tiles_t& tiles, const position_t& from_where,
        int_matrix_t& on_what, int with_what)
{
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    int target_color = tiles[from_where.x][from_where.y];
    positions_t positions;

    std::vector<position_t> position_stack;
    position_stack.push_back(from_where);

    while (!position_stack.empty()) {
        position_t top = position_stack.back();
        position_stack.pop_back();

        on_what[top.x][top.y] = with_what;
        positions.push_back(position_t{top.x, top.y});

        if (top.x > 0 &&
            tiles[top.x - 1][top.y] == target_color &&
            on_what[top.x - 1][top.y] != with_what)
        {
            position_stack.push_back(position_t{top.x - 1, top.y});
        }
        if (top.x < columns - 1 &&
            tiles[top.x + 1][top.y] == target_color &&
            on_what[top.x + 1][top.y] != with_what)
        {
            position_stack.push_back(position_t{top.x + 1, top.y});
        }
        if (top.y > 0 &&
            tiles[top.x][top.y - 1] == target_color &&
            on_what[top.x][top.y - 1] != with_what)
        {
            position_stack.push_back(position_t{top.x, top.y - 1});
        }
        if (top.y < rows - 1 &&
            tiles[top.x][top.y + 1] == target_color &&
            on_what[top.x][top.y + 1] != with_what)
        {
            position_stack.push_back(position_t{top.x, top.y + 1});
        }
    }
    return positions;
}

void do_montecarlo(const tiles_t& tiles_copy) {

    tiles_t tiles = tiles_copy;

    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    for (unsigned i = 0; i < 100; ++i) {
        islands_t islands = get_islands(tiles);

        std::cerr << i << ": island_count = " <<  islands.size() << std::endl;
        if (islands.size() == 3) {
            std::cerr << "we're done" << std::endl;
        }

        std::sort(islands.begin(), islands.end(),
                [](const island_t& lhs, const island_t& rhs) {
                    return lhs.positions.size() < rhs.positions.size();
                });

        swaps_t swaps = get_swaps(tiles, islands);
        unsigned best_swap = 0;
        unsigned best_score = score_tiles(run_montecarlo(tiles, swaps[0]));

        for (unsigned j = 1; j < swaps.size(); ++j) {
            unsigned score = score_tiles(run_montecarlo(tiles, swaps[j]));
            if (score > best_score) {
                best_score = score;
                best_swap = j;
            }
        }
        swap_tiles(tiles, swaps[best_swap]);
        print_swap(swaps[best_swap]);
    }
}

void swap_tiles(tiles_t& tiles, const swap_t& swap) {
    std::swap(tiles[std::get<0>(swap).x][std::get<0>(swap).y],
            tiles[std::get<1>(swap).x][std::get<1>(swap).y]);
}

swaps_t get_swaps(const tiles_t& /*tiles*/, const islands_t& islands) {
    swaps_t swaps;
    for (unsigned j = 0; j < 10 && j < islands.size(); ++j) {
        for (unsigned k = 0; k < 10 && k < islands.size(); ++k) {
            if (islands[j].color != islands[k].color) {
                swaps.push_back(swap_t(islands[j].positions[0], islands[k].positions[0]));
            }
        }
    }
    return swaps;
}

tiles_t run_montecarlo(tiles_t tiles, const swap_t& swap, int depth) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    position_t p1, p2;
    std::tie(p1, p2) = swap;

    std::swap(tiles[p1.x][p1.y], tiles[p2.x][p2.y]);

    for (int i = 0; i < depth; ++i) {
        unsigned x1 = std::rand() % columns;
        unsigned y1 = std::rand() % rows;
        unsigned x2 = std::rand() % columns;
        unsigned y2 = std::rand() % rows;

        std::swap(tiles[x1][y1], tiles[x2][y2]);
    }

    return tiles;
}

void print_swap(const swap_t& swap) {
    std::cout << std::get<0>(swap).y << " " << std::get<0>(swap).x << " " <<
                 std::get<1>(swap).y << " " << std::get<1>(swap).x << std::endl;
}

unsigned score_tiles(const tiles_t& tiles) {
    return get_islands(tiles).size();
}

void do_graph(const tiles_t& tiles) {
    graph_t full_graph = create_graph(tiles);

    std::array<graph_t, 3> color_grahs {{
        get_color_graph(full_graph, 0),
        get_color_graph(full_graph, 1),
        get_color_graph(full_graph, 2)
    }};
    //TODO not finished
}

void do_bit_magic(const tiles_t& tiles) {
    bit_tiles_t bit_tiles = to_bit_tiles(tiles);
}

void do_naive(const tiles_t& tiles) {
    //TODO BELA
}

bit_tiles_t to_bit_tiles(const tiles_t& tiles) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    assert(columns == 64);
    assert(rows == 64);

    bit_tiles_t result = {{{0}, {0}, {0}}};

    for (unsigned y = 0; y < 64; ++y) {
        for (unsigned x = 0; x < 64; ++x) {
            set_bit(result[tiles[x][y]], {x, y});
        }
    }

    return result;
}

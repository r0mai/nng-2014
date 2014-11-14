
#include <iostream>
#include <tuple>
#include <cstdlib>
#include <cmath>

#include <boost/optional.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include "common.hpp"

bool operator==(const position_t& lhs, const position_t& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

tiles_t read_from(std::istream& in) {
    unsigned rows, columns;

    in >> rows >> columns;

    tiles_t tiles(boost::extents[columns][rows]);

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            in >> tiles[x][y];
        }
    }

    return tiles;
}

void print_swaps(const swaps_t& swaps, std::ostream& out) {
    out << swaps.size() << "\n";
    for (auto p : swaps) {
        out << get<0>(p).y << " " << get<0>(p).x << " "
            << get<1>(p).y << " " << get<1>(p).x << "\n";
    }
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

void print_tiles(const tiles_t& tiles) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            switch (tiles[x][y]) {
            case 0:
                std::cerr << "\033[31m"; break;
            case 1:
                std::cerr << "\033[32m"; break;
            case 2:
                std::cerr << "\033[34m"; break;
            }
            std::cerr << "X";
            std::cerr << "\033[0m";
        }
        std::cerr << std::endl;
    }
}

void print_tiles_as_input(const tiles_t& tiles) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    std::cout << rows << " " << columns << std::endl;
    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            std::cout << tiles[x][y] << " ";
        }
        std::cout << std::endl;
    }
}

bool is_done(const tiles_t& tiles) {
    //TODO optimize
    return get_islands(tiles).size() == 3;
}

namespace {
    int index_into_other(
            const tiles_t& index, const int& indexee, const tiles_t& into)
    {
        return into.data()[&indexee - index.data()];
    }
}

swaps_t get_swaps(const tiles_t& original, const tiles_t& result) {
    //Based on Bela's algorithm

    std::array<std::vector<position_t>, 3> areas;

    unsigned columns = original.shape()[0];
    unsigned rows = original.shape()[1];

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            areas[result[x][y]].push_back({x, y});
        }
    }

    auto begin0 = areas[0].begin();
    auto begin1 = areas[1].begin();
    auto begin2 = areas[2].begin();
    auto end0 = areas[0].end();
    auto end1 = areas[1].end();
    auto end2 = areas[2].end();

    swaps_t swaps;

    auto get_original = [&](position_t p) { return original[p.x][p.y]; };
    auto my_swap = [&](auto lhs, auto rhs) {
        std::swap(*lhs, *rhs);
        swaps.push_back(swap_t{*lhs, *rhs});
    };

    while( begin0 < end0 || begin1 < end1 )
    {
        while ( begin0 < end0 && get_original(*begin0) == 0 ) ++begin0;
        while ( begin1 < end1 && get_original(*begin1) == 1 ) ++begin1;
        while ( begin2 < end2 && get_original(*begin2) == 2 ) ++begin2;
        if( begin0 < end0 && get_original(*begin0) == 1 )
        {
            auto it = begin1;
            while( it < end1 && get_original(*it) != 0 ) ++it;
            if( it == end1 )
            {
                my_swap( begin0 , it = begin1 );
            }
            else
            {
                my_swap( begin0, it );
                ++begin0;
            }
            if( it == begin1 ) ++begin1;
        }
        else if( begin0 < end0 && get_original(*begin0) == 2 )
        {
            auto it = begin2;
            while( it < end2 && get_original(*it) != 0 ) ++it;
            if( it == end2 )
            {
                my_swap( begin0 , it = begin2 );
            }
            else
            {
                my_swap( begin0, it );
                ++begin0;
            }
            if( it == begin2 ) ++begin2;
        }
        else if( begin1 < end1 && begin2 < end2 )
        {
            my_swap( begin1 , begin2 );
            begin1++;
            begin2++;
        }
        else
        {
            break;
        }
    }
    return swaps;
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


// REALM OF CELLS

void do_cellular(const tiles_t& tiles) {
    CellularRunner runner(tiles);
    runner.run();
}

swap_t CellularRunner::get_best_swap() {
    std::array<std::pair<swap_t, unsigned>, 3> swaps =
    {{
        std::make_pair(swap_t(get_minimal(0), get_maximal(0)), 0),
        std::make_pair(swap_t(get_minimal(1), get_maximal(1)), 1),
        std::make_pair(swap_t(get_minimal(2), get_maximal(2)), 2)
    }};

    std::sort(swaps.begin(), swaps.end(),
            [&](auto x, auto y) -> bool {
                return
                score_matrix
                [std::get<1>(x.first).x][std::get<1>(x.first).y][x.second] -
                score_matrix
                [std::get<0>(x.first).x][std::get<0>(x.first).y][x.second] <
                score_matrix
                [std::get<1>(y.first).x][std::get<1>(y.first).y][y.second] -
                score_matrix
                [std::get<0>(y.first).x][std::get<0>(y.first).y][y.second];
            });
    return swaps.back().first;
}

position_t CellularRunner::get_minimal(int color) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    boost::optional<position_t> minimal;

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            if (tiles[x][y] != color) {
                continue;
            }
            if (!minimal || score_matrix[minimal->x][minimal->y][color] >
                    score_matrix[x][y][color])
            {
                minimal = {x, y};
            }
        }
    }
    return *minimal;
}

position_t CellularRunner::get_maximal(int color) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    boost::optional<position_t> maximal;

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            if (tiles[x][y] == color) {
                continue;
            }
            if (!maximal || score_matrix[maximal->x][maximal->y][color] <
                    score_matrix[x][y][color])
            {
                maximal = {x, y};
            }
        }
    }
    return *maximal;
}

void CellularRunner::run() {
    print_tiles(tiles);

    std::cerr << "Initial islands = " << get_islands(tiles).size() << std::endl;

    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    get_score_matrix();

    for (int i = 0; i < 2000 /*&& !is_done(tiles)*/; ++i) {
        swap_t swap = get_best_swap();
        do_swap(swap);
        update_score_matrix_around(std::get<0>(swap));
        update_score_matrix_around(std::get<1>(swap));
        if (i % 100 == 0) {
            print_tiles(tiles);
            std::cerr << i << " : " << get_islands(tiles).size() << std::endl;
        }
    }

    std::cerr << "\n----------\n";
    print_tiles(tiles);

    std::cerr << "Finished islands = " << get_islands(tiles).size() << std::endl;
    print_tiles_as_input(tiles);
}

CellularRunner::score_t CellularRunner::get_score(const position_t& pos) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    score_t score = {{0, 0, 0}};
    for (int i = 0; i < 3; ++i) {
        unsigned min_x = std::max(pos.x, radius) - radius;
        unsigned min_y = std::max(pos.y, radius) - radius;
        unsigned max_x = std::min(pos.x + radius, columns - 1);
        unsigned max_y = std::min(pos.y + radius, rows - 1);

        for (unsigned x = min_x; x <= max_x; ++x) {
            for (unsigned y = min_y; y <= max_y; ++y) {
                unsigned d = std::abs(int(x) - int(pos.x)) + std::abs(int(y) - int(pos.y));
                if (d > radius || tiles[x][y] != i) continue;

                score[i] += (1ull << (radius - d));
            }
        }
    }
    return score;
}

void CellularRunner::get_score_matrix() {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            score_matrix[x][y] = get_score({x, y});
        }
    }
}

void CellularRunner::update_score_matrix_around(const position_t& pos) {
    unsigned columns = tiles.shape()[0];
    unsigned rows = tiles.shape()[1];

    for (unsigned y = std::max(radius, pos.y) - radius; y < rows && y < pos.y + radius+1; ++y) {
        for (unsigned x = std::max(radius, pos.x) - radius; x < columns && x < pos.x + radius+1; ++x) {
            score_matrix[x][y] = get_score({x, y});
        }
    }
}

void CellularRunner::do_swap(const swap_t& swap) {
    swap_tiles(tiles, swap);
    swaps.push_back(swap);
}

void CellularRunner::print_swaps() {
    ::print_swaps(swaps);
}

swaps_t do_from_to(tiles_t from, const tiles_t& to) {
    unsigned columns = from.shape()[0];
    unsigned rows = from.shape()[1];

    swaps_t swaps;
    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            if (from[x][y] != to[x][y]) {
                int needed_color = to[x][y];
                for (unsigned y2 = y; y2 < rows; ++y2) {
                    for (unsigned x2 = x; x2 < columns; ++x2) {
                        if (from[x2][y2] != to[x2][y2] &&
                            from[x2][y2] == needed_color)
                        {
                            std::swap(from[x][y], from[x2][y2]);
                            swaps.push_back(swap_t({x, y}, {x2, y2}));
                            goto continue_outer;
                        }
                    }
                }
                continue_outer: ;
            }
        }
    }
    return swaps;
}



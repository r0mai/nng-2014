
#include <iostream>

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

void do_montecarlo(const tiles_t& tiles) {
    //TODO implement
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

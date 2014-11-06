
#include <vector>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

struct vertex_property_tag {
    typedef boost::vertex_property_tag kind;
};
struct edge_property_tag {
    typedef boost::edge_property_tag kind;
};

struct vertex_property {
    unsigned x, y;
    int color;
};
struct edge_property {
};

typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::undirectedS,
    boost::property<vertex_property_tag, vertex_property>,
    boost::property<edge_property_tag, edge_property>> graph_t;

typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor;

typedef boost::graph_traits<graph_t>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<graph_t>::edge_iterator edge_iterator;
typedef boost::graph_traits<graph_t>::in_edge_iterator in_edge_iterator;
typedef boost::graph_traits<graph_t>::out_edge_iterator out_edge_iterator;

typedef std::vector<std::vector<int>> tiles_t;

vertex_property& get_vertex_property(vertex_descriptor vertex, graph_t& graph) {
    return boost::get(vertex_property_tag(), graph, vertex);
}

edge_property& get_edge_property(edge_descriptor edge, graph_t& graph) {
    return boost::get(edge_property_tag(), graph, edge);
}

graph_t create_graph(const tiles_t& tiles) {
    typedef std::vector<std::vector<vertex_descriptor>> vertex_matrix_t;

    unsigned rows = tiles[0].size();
    unsigned columns = tiles.size();

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

int main() {
    unsigned rows, columns;

    std::cin >> rows >> columns;

    tiles_t tiles(columns, std::vector<int>(rows));

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            std::cin >> tiles[x][y];
        }
    }

    graph_t full_graph = create_graph(tiles);

    std::array<graph_t, 3> color_grahs {{
        get_color_graph(full_graph, 0),
        get_color_graph(full_graph, 1),
        get_color_graph(full_graph, 2)
    }};
}

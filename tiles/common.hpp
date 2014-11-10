#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <cassert>

#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <boost/multi_array.hpp>

typedef boost::multi_array<int, 2> int_matrix_t;
typedef boost::multi_array<bool, 2> bool_matrix_t;

typedef int_matrix_t tiles_t;
typedef int_matrix_t island_map_t;


struct position_t {
    unsigned x, y;
};

typedef std::vector<position_t> positions_t;

typedef std::tuple<position_t, position_t> swap_t;
typedef std::vector<swap_t> swaps_t;

struct island_t {
    island_t() = default;
    island_t(int color, unsigned index, const positions_t& positions) :
        color(color), index(index), positions(positions) {}

    int color = -1;
    unsigned index;
    positions_t positions;
};

typedef std::vector<island_t> islands_t;

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

tiles_t read_from_cin();
vertex_property& get_vertex_property(vertex_descriptor vertex, graph_t& graph);
edge_property& get_edge_property(edge_descriptor edge, graph_t& graph);
graph_t create_graph(const tiles_t& tiles);
graph_t get_color_graph(graph_t graph, int color);

void print_tiles(const tiles_t& tiles);

bool is_done(const tiles_t& tiles);

islands_t get_islands(const tiles_t& tiles);

positions_t flood_and_paint(const tiles_t& tiles, const position_t& from_where,
        int_matrix_t& on_what, int with_what);

void swap_tiles(tiles_t& tiles, const swap_t& swap);

swaps_t get_swaps(const tiles_t& tiles, const islands_t& islands);

tiles_t run_montecarlo(tiles_t tiles, const swap_t& swap, int depth = 30);

void print_swap(const swap_t& swap);

unsigned score_tiles(const tiles_t& tiles);

void do_montecarlo(const tiles_t& tiles);
void do_graph(const tiles_t& tiles);
void do_naive(const tiles_t& tiles);
void do_bit_magic(const tiles_t& tiles);
void do_cellular(const tiles_t& tiles);

//bit magic

typedef std::uint64_t bit_row_t;
typedef std::array<bit_row_t, 64> bit_matrix_t;
typedef std::array<bit_matrix_t, 3> bit_tiles_t;

bit_tiles_t to_bit_tiles(const tiles_t& tiles);

inline
void set_bit(bit_matrix_t& m, const position_t& p) {
    m[p.y] |= (1 << p.x);
}

inline
void utdnset_bit(bit_matrix_t& m, const position_t& p) {
    m[p.y] &= ~(1 << p.x);
}

inline bool get_bit(const bit_matrix_t& m, const position_t& p) {
    return m[p.y] & (1 << p.x);
}

#endif

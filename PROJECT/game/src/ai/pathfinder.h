#pragma once
#include "../grid/grid.h"

/*
Class containing an implementation of the A star algorithm operating on
the tiles of a grid object.
*/
class pathfinder {
	typedef std::pair<int, int> index;

	/*
	Definition of a node in the graph. f,g and h have their usual meanings for the
	A star algorithm
	*/
	struct node {
		index pos{0,0};
		int f{0};
		int g{0};
		int h{0};
		index parent{0,0};
		node() {};
		node(index _pos, int _f, int _g, int _h, index _parent) :pos{_pos}, f{ _f }, g{ _g }, h{ _h }, parent{ _parent } {};
		~node() {};
	};

	/*
	Allows sorting of nodes according to their f cost, for use in a priority queue
	*/
	struct node_comparator {
		bool operator()(node& a, node& b) { return a.f > b.f; };
	};

public:
	static std::deque<glm::vec3> find_path(const grid& level_grid);


};

#pragma once
#include "../grid/grid.h"

class pathfinder {
	typedef std::pair<int, int> index;

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

	struct node_comparator {
		bool operator()(node& a, node& b) { return a.f > b.f; };
	};

public:
	static std::deque<glm::vec3> find_path(const grid& level_grid);

	
private:
	

};

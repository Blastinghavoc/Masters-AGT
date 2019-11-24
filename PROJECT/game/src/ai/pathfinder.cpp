#include "pch.h"
#include "pathfinder.h"
#include <set>

/*
A-star algorithm, based on description of it found here: https://www.geeksforgeeks.org/a-search-algorithm/

*/
std::deque<glm::vec3> pathfinder::find_path(const grid& level_grid)
{
	const index start = level_grid.get_start();
	const index end = level_grid.get_end();
	std::priority_queue<node, std::vector<node>, node_comparator> open_list;
	std::set<index> closed_list;
	std::map<index, node> current_values;

	//shorthand function
	auto closed_contains = [&closed_list](const index& i) {return closed_list.count(i) > 0; };

	//manhattan distance calculation
	auto distance = [](const index& i, const index& j) {return abs(j.first - i.first) + abs(j.second - i.second); };

	//utility function to create a child of a node
	auto init_child = [&distance,&end](const node& parent, const index& child_index) {
		auto g_child = parent.g + 1;
		auto h_child = distance(child_index, end);
		return node(child_index,g_child+h_child,g_child,h_child,parent.pos);
	};

	//obtain all valid (pathable) children of the input node
	auto get_all_children = [&level_grid,&closed_contains,&init_child](const node& nd) {
		std::vector<node> children;
		int x = nd.pos.first;
		int z = nd.pos.second;

		for (index& i : std::vector<index>{index(x+1,z),index(x,z+1) ,index(x - 1,z) ,index(x,z - 1) })
		{
			if (level_grid.is_walkable(i) && ! closed_contains(i))
			{
				children.push_back(init_child(nd,i));
			}
		}
		return children;
	};

	/*Function to obtain the final path as a series of positions once the goal has been found.
	Only includes turning points on the path to reduce the size of the resulting deque.
	*/
	auto get_path = [&current_values,&end,&start,&level_grid]() {
		std::deque<glm::vec3> path;
		auto tmp_node = current_values[end];
		auto offset = 0.5f * glm::vec3(level_grid.cell_size(), 0, level_grid.cell_size());//Add half the cell size to get center of square

		path.push_front(level_grid.grid_to_world_coords(tmp_node.pos.first, tmp_node.pos.second) + offset);
		auto prev_pos = tmp_node.pos;
		tmp_node = current_values[tmp_node.parent];

		while (tmp_node.pos != start)
		{
			//If the next node is not inline with the previous one, then the current node is a junction, so place a waypoint
			if (tmp_node.parent.first != prev_pos.first && tmp_node.parent.second != prev_pos.second)
			{
				path.push_front(level_grid.grid_to_world_coords(tmp_node.pos.first, tmp_node.pos.second) + offset);
			}
			prev_pos = tmp_node.pos;
			tmp_node = current_values[tmp_node.parent];
		}
		//Add the start
		path.push_front(level_grid.grid_to_world_coords(tmp_node.pos.first, tmp_node.pos.second) + offset);

		return path;
	};

	//Actual algorithm
	if (start != end)
	{

		auto start_node = node(start, 0, 0, 0, start);
		open_list.emplace(start_node);
		current_values[start_node.pos] = start_node;

		while (!open_list.empty())
		{
			node current_node = open_list.top();
			open_list.pop();
			if (closed_contains(current_node.pos))
			{
				//Node has already been processed
				continue;
			}

			if (current_node.pos == end)
			{
				return get_path();
			}

			for (auto& child: get_all_children(current_node))
			{
				bool add = false;
				if (current_values.count(child.pos) > 0)
				{
					if (current_values[child.pos].f > child.f)
					{
						add = true;
					}
				}
				else {
					add = true;
				}

				if (add)
				{
					current_values[child.pos] = child;
					open_list.emplace(child);
				}
			}

			closed_list.emplace(current_node.pos);
		}

	}

	return std::deque<glm::vec3>();
}

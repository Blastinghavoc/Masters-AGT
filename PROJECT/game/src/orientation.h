#pragma once
#include <engine.h>

/*
Class used rather like an enum, but with methods.
Defines 8 global orientations, four cardinal and four composite,
as well as operations on them.
Used to simplify notation when reasoning about directions.
*/
class orientation {
public:
	//Cardinal
	static const orientation north;
	static const orientation east;
	static const orientation south;
	static const orientation west;
	//Composite
	static const orientation north_east;
	static const orientation south_east;
	static const orientation south_west;
	static const orientation north_west;

	//Convert to a vector. E.g North is {0,0,1}
	glm::vec3 to_vec() const;

	//Invert the direction, ie north.invert() = south
	orientation invert() const;

	//For a cardinal orientation, return the two composite components. E.g north-> {north_east,north_west}
	std::vector<orientation> composite_components() const;

	//For a composite orientation, return the two cardinal components. E.g north-east -> {north, east}
	std::vector<orientation> cardinal_components() const;

	//Returns true if the orientation is cardinal (north,south, etc), false otherwise (north_west...)
	bool is_cardinal() const { return m_is_cardinal; };

	//Default and copy constructor
	orientation(const orientation& o = north) :m_value{ o.m_value }, m_is_cardinal{o.m_is_cardinal} {};
	
	~orientation();

	static std::vector<orientation> get_all_cardinal() { return { north,east,south,west }; };
	static std::vector<orientation> get_all_composite() { return { north_east,south_east,south_west,north_west }; };
private:
    //Private constructor to prevent creation of any orientations not specified above
	orientation(unsigned short val,bool is_card);

	const unsigned short m_value;
	const bool m_is_cardinal;

	//Representations of each orientation for comparison operation purposes
	static constexpr unsigned short north_val = 0;
	static constexpr unsigned short east_val = 1;
	static constexpr unsigned short south_val = 2;
	static constexpr unsigned short west_val = 3;
	static constexpr unsigned short north_east_val = 4;
	static constexpr unsigned short south_east_val = 5;
	static constexpr unsigned short south_west_val = 6;
	static constexpr unsigned short north_west_val = 7;

public:
	//Operators
	bool equal_op(const orientation& o2) const { return (this->m_value == o2.m_value); }
	bool lt_op(const orientation& o2) const { return (this->m_value < o2.m_value); }
};

//Full set of relational operators, defined in terms of == and <
inline bool operator==(const orientation& o1,const orientation& o2) { return o1.equal_op(o2); }
inline bool operator<(const orientation& o1, const orientation& o2) { return o1.lt_op(o2); }
inline bool operator<=(const orientation& o1, const orientation& o2) { return (o1 < o2 || o1 == o2); }
inline bool operator!=(const orientation& o1, const orientation& o2) { return !(o1 == o2); }
inline bool operator>(const orientation& o1, const orientation& o2) { return o2 < o1; }
inline bool operator>=(const orientation& o1, const orientation& o2) { return (o1 > o2 || o1 == o2); }

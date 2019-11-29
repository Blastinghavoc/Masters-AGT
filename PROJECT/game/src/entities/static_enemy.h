#pragma once

#include "abstract_enemy.h"

/*
Represents a non-animated enemy, i.e an enemy with a static model.
NOT an enemy that doesn't move.
*/
class static_enemy : public abstract_enemy {
public:	

	static_enemy();
	static_enemy(int id, engine::ref<engine::game_object> obj,enemy_type type = enemy_type::robot1);
	~static_enemy() {};

	virtual void on_update(const engine::timestep& time_step) override;		
};

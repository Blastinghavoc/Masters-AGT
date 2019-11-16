#pragma once

#include "animated_actor.h"
#include "trigger_box.h"
#include "abstract_enemy.h"

class animated_enemy: public animated_actor, public abstract_enemy {
public:
	animated_enemy();
	animated_enemy(int id,glm::vec3 position = {0,0,0});
	~animated_enemy();

	void on_update(const engine::timestep& time_step) override;

	static engine::ref<animated_enemy> create(int id, glm::vec3 position = { 0,0,0 }) {
		return std::make_shared<animated_enemy>(id, position);
	};

private:
	static engine::game_object_properties prefab;
	static bool prefab_ready;
};

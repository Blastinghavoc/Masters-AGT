#pragma once
#include "abstract_actor.h"
#include "trigger_box.h"

/*
Class representing an object that can be interacted with in the scene.
It has a trigger box and a message to be displayed while that trigger box
is in range of the player's trigger box.
This is currently only used for the hard-mode lever in the middle of the map
*/
class interactable : public abstract_actor {
public:
	interactable() {};
	interactable(engine::ref<engine::game_object> obj, std::string message = "");
	~interactable() {};

	void on_update(const engine::timestep& time_step) override {
		m_box.on_update(m_object->position());
	};

	bool in_range_of(trigger_box& other) {
		return m_box.collision(other);
	};

	std::string message() { return m_interaction_message; };

private:
	trigger_box m_box;
	std::string m_interaction_message;
};

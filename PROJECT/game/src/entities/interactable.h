#pragma once
#include "abstract_actor.h"
#include "trigger_box.h"

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

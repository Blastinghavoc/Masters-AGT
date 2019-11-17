#include "pch.h"
#include "interactable.h"

interactable::interactable(engine::ref<engine::game_object> obj, std::string message) :m_interaction_message{message}
{
	m_object = obj;
	m_box.set_box(m_object);
}

#include "pch.h"
#include "gameplay_manager.h"

int gameplay_manager::m_score = 0;
int gameplay_manager::m_health = 100;
int gameplay_manager::m_money = 0;
engine::timer gameplay_manager::m_build_timer{};
int gameplay_manager::m_max_build_time = 30;
std::map<std::string, int> gameplay_manager::m_prices{};
player* gameplay_manager::m_player_ptr;
glm::vec3  gameplay_manager::m_player_spawnpoint{};
engine::ref<text_hud_element> gameplay_manager::m_time_display{};
engine::ref<text_hud_element> gameplay_manager::m_score_display{};
engine::ref<text_hud_element> gameplay_manager::m_money_display{};
engine::ref<text_hud_element> gameplay_manager::m_life_display{};

void gameplay_manager::init(player* playr,engine::ref<engine::text_manager> text_manager)
{
	m_player_ptr = playr;
	m_player_spawnpoint = m_player_ptr->object()->position();

	//TODO store the crosshair somewhere?
	auto y_scale = (float)engine::application::window().width() / (float)engine::application::window().height();
	auto crosshair = hud_element::create(glm::vec2(.5f, .5f),
		{ 0.025f,0.025f * y_scale },
		engine::texture_2d::create("assets/textures/crosshair.png", true));
	hud_manager::add_element(crosshair);

	text_hud_element::default_colour = { .5f, 1.f, 0.5f, 1.f };

	m_time_display = text_hud_element::create(text_manager, "Time Remaining: ", glm::vec2{ 0.4f,0.95f });
	m_time_display->set_text_size(0.5f);
	hud_manager::add_element(m_time_display);

	m_score_display = text_hud_element::create(text_manager, "Score: ", glm::vec2{ 0.025f,0.95f });
	m_score_display->set_text_size(0.5f);
	hud_manager::add_element(m_score_display);

	m_money_display = text_hud_element::create(text_manager, "Money: ", glm::vec2{ 0.025f,0.9f });
	m_money_display->set_text_size(0.5f);
	hud_manager::add_element(m_money_display);

	m_life_display = text_hud_element::create(text_manager, "Health: ", glm::vec2{ 0.85f,0.95f });
	m_life_display->set_text_size(0.5f);
	hud_manager::add_element(m_life_display);
}

void gameplay_manager::update(const engine::timestep& ts)
{
	m_money_display->set_text("Money: "+std::to_string(m_money));
	m_life_display->set_text("Health: " + std::to_string(m_health));
	m_score_display->set_text("Score: "+ std::to_string(m_score));

	int time_remaining = build_time();
	if (time_remaining > 0)
	{
		m_time_display->set_text("Time: "+ std::to_string(time_remaining));
	}
	else {
		m_time_display->set_text("");
	}
}

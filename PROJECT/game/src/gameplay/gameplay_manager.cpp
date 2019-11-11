#include "pch.h"
#include "gameplay_manager.h"

//Static initializers
int gameplay_manager::m_score = 0;
int gameplay_manager::m_health = 100;
int gameplay_manager::m_money = 0;
int gameplay_manager::m_portal_health = 100;
engine::timer gameplay_manager::m_build_timer{};
int gameplay_manager::m_max_build_time = 30;
std::map<std::string, int> gameplay_manager::m_prices{};
player* gameplay_manager::m_player_ptr;
glm::vec3  gameplay_manager::m_player_spawnpoint{};
engine::ref<text_hud_element> gameplay_manager::m_top_display{};
engine::ref<text_hud_element> gameplay_manager::m_score_display{};
engine::ref<text_hud_element> gameplay_manager::m_money_display{};
engine::ref<text_hud_element> gameplay_manager::m_health_display{};
engine::ref<text_hud_element> gameplay_manager::m_portal_health_display{};
bool gameplay_manager::m_wave_active = false;
std::vector<gameplay_manager::wave_definition> gameplay_manager::m_waves{};
int gameplay_manager::m_max_waves = 0;
int gameplay_manager::m_wave_number = 0;

void gameplay_manager::init(player* playr,engine::ref<engine::text_manager> text_manager)
{
	m_player_ptr = playr;
	m_player_spawnpoint = m_player_ptr->object()->position();

	//TODO store the crosshair somewhere? Only if I ever need to modify it.
	auto y_scale = (float)engine::application::window().width() / (float)engine::application::window().height();
	auto crosshair = hud_element::create(glm::vec2(.5f, .5f),
		{ 0.025f,0.025f * y_scale },
		engine::texture_2d::create("assets/textures/crosshair.png", true));
	hud_manager::add_element(crosshair);

	text_hud_element::default_colour = { .5f, 1.f, 0.5f, 1.f };

	m_top_display = text_hud_element::create(text_manager, "Time Remaining: ", glm::vec2{ 0.4f,0.95f });
	m_top_display->set_text_size(0.5f);
	hud_manager::add_element(m_top_display);

	m_score_display = text_hud_element::create(text_manager, "Score: ", glm::vec2{ 0.025f,0.95f });
	m_score_display->set_text_size(0.5f);
	hud_manager::add_element(m_score_display);

	m_money_display = text_hud_element::create(text_manager, "Money: ", glm::vec2{ 0.025f,0.9f });
	m_money_display->set_text_size(0.5f);
	hud_manager::add_element(m_money_display);

	m_health_display = text_hud_element::create(text_manager, "Health: ", glm::vec2{ 0.82f,0.9f });
	m_health_display->set_text_size(0.5f);
	hud_manager::add_element(m_health_display);

	m_portal_health_display = text_hud_element::create(text_manager, "Portal Health: ", glm::vec2{ 0.82f,0.95f });
	m_portal_health_display->set_text_size(0.5f);
	hud_manager::add_element(m_portal_health_display);

	//Adding waves in reverse order
	m_waves.push_back({ 15,3,30 });
	m_waves.push_back({ 5,1,10 });
	m_waves.push_back({ 10,3,30 });
	m_waves.push_back({ 10,5,30 });
	m_waves.push_back({5,5,30});

	m_max_waves = (int)m_waves.size();
}

void gameplay_manager::update(const engine::timestep& ts)
{
	m_money_display->set_text("Money: "+std::to_string(m_money));
	m_health_display->set_text("Health: " + std::to_string(m_health));
	m_score_display->set_text("Score: "+ std::to_string(m_score));
	m_portal_health_display->set_text("Portal Health: " + std::to_string(m_portal_health));

	if (m_wave_active)
	{
		auto total_enemies_this_wave = m_waves.back().num_enemies;
		auto spawned_so_far = total_enemies_this_wave - enemy_manager::remaining();
		auto alive = enemy_manager::current_active();
		m_top_display->set_text("Wave: " + std::to_string(m_wave_number) + " of " + std::to_string(m_max_waves)
			+ " Enemies: " + std::to_string(spawned_so_far) + " of " +
			std::to_string(total_enemies_this_wave) + " alive: " +std::to_string(alive)
		);
	}
	else {
		int time_remaining = build_time();
		LOG_INFO("{},{},{}",time_remaining,m_build_timer.total(),m_build_timer.elapsed());
		if (time_remaining > 0)
		{
			m_top_display->set_text("Time: "+ std::to_string(time_remaining));
		}
		else {			
			m_build_timer.reset();
			m_wave_active = true;
			
			enemy_manager::begin_wave(m_waves.back().num_enemies, m_waves.back().enemy_spacing);
		}
	}

}

void gameplay_manager::begin_wave()
{
	if (!m_waves.empty())
	{
		auto wave_config = m_waves.back();
		m_waves.pop_back();
		m_max_build_time = wave_config.prep_time;
		//Start build phase
		m_build_timer.start();
		m_wave_active = false;
		++m_wave_number;
	}
	else {
		//TODO display victory
	}
}

#include "pch.h"
#include "gameplay_manager.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"

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
engine::perspective_camera* gameplay_manager::m_camera;
engine::ref<grid> gameplay_manager::m_level_grid;
gameplay_manager::wave_definition gameplay_manager::m_current_wave_definition;

void gameplay_manager::init(player* playr,engine::ref<engine::text_manager> text_manager, engine::perspective_camera* camera,
	engine::ref<grid> level_grid)
{
	m_player_ptr = playr;
	m_player_spawnpoint = m_player_ptr->object()->position();

	m_camera = camera;
	m_level_grid = level_grid;

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
		auto total_enemies_this_wave = m_current_wave_definition.num_enemies;
		auto spawned_so_far = total_enemies_this_wave - enemy_manager::remaining();
		auto alive = enemy_manager::current_active();
		m_top_display->set_text("Wave: " + std::to_string(m_wave_number) + " of " + std::to_string(m_max_waves)
			+ " | Enemies: " + std::to_string(spawned_so_far) + " of " +
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
			start_combat_phase();
		}
	}

}

//Begins the next build phase, which will trigger the next wave when it finishes
void gameplay_manager::next_build_phase()
{
	if (!m_waves.empty())
	{
		auto wave_config = m_waves.back();
		m_waves.pop_back();
		m_current_wave_definition = wave_config;
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

void gameplay_manager::on_event(engine::event& event)
{
	if (event.event_type() == engine::event_type_e::mouse_button_pressed)
	{
		auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);
		switch (e.mouse_button())
		{
		case engine::mouse_button_codes::MOUSE_BUTTON_1:
			mouse1_event_handler();
			e.handled = true;
			break;
		case engine::mouse_button_codes::MOUSE_BUTTON_2:
			mouse2_event_handler();
			e.handled = true;
			break;
		default:
			break;
		}
	}

	if (!event.handled && event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		switch (e.key_code())
		{		
		case engine::key_codes::KEY_ENTER:
			//Allows build phase to be skipped early.			
			start_combat_phase();
			break;
		default:
			break;
		}
	}
}

/*
TODO: Add logic for weapons as well, and switching between build and combat mode.
Refactor duplicated code.
*/
//Remove a block at the targeted position (if possible)
void gameplay_manager::mouse1_event_handler()
{
	if (!m_wave_active)
	{
		auto fv = m_camera->front_vector();
		if (fv.y > 0)
		{
			//Not looking at the ground
			return;
		}
		auto cam_pos = m_camera->position();
		auto delta_y = m_level_grid->floor_level() - cam_pos.y;
		auto ground_pos = cam_pos + (delta_y / fv.y) * fv;
		auto grid_coords = m_level_grid->world_to_grid_coords(ground_pos);
		m_level_grid->remove_block(grid_coords.first, grid_coords.second);
	}
	else {
		//TODO Fire weapon
	}
}

//Place a block at the targeted position if posible
void gameplay_manager::mouse2_event_handler()
{
	if (!m_wave_active)
	{
		auto fv = m_camera->front_vector();
		if (fv.y > 0)
		{
			//Not looking at the ground
			return;
		}
		auto cam_pos = m_camera->position();
		auto delta_y = m_level_grid->floor_level() - cam_pos.y;
		auto ground_pos = cam_pos + (delta_y / fv.y) * fv;
		auto grid_coords = m_level_grid->world_to_grid_coords(ground_pos);
		m_level_grid->place_block(grid_coords.first, grid_coords.second);
		
		if (pathfinder::find_path(*m_level_grid).empty())
		{
			//Placing block blocked the path, so undo
			m_level_grid->remove_block(grid_coords.first, grid_coords.second);
		}
	}
}

void gameplay_manager::start_combat_phase()
{
	m_build_timer.reset();
	m_wave_active = true;

	enemy_manager::begin_wave(m_current_wave_definition.num_enemies, m_current_wave_definition.enemy_spacing);	
}

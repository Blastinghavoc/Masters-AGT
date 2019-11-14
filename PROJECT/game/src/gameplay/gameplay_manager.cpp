#include "pch.h"
#include "gameplay_manager.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"
#include "../ai/turret_manager.h"
#include "../sfx/sfx_manager.h"

//Static initializers
int gameplay_manager::m_score = 0;
int gameplay_manager::m_health = 100;
int gameplay_manager::m_money = 100;
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
engine::ref<text_hud_element> gameplay_manager::m_tool_display{};
bool gameplay_manager::m_wave_active = false;
std::vector<gameplay_manager::wave_definition> gameplay_manager::m_waves{};
int gameplay_manager::m_max_waves = 0;
int gameplay_manager::m_wave_number = 0;
engine::perspective_camera* gameplay_manager::m_camera;
engine::ref<grid> gameplay_manager::m_level_grid;
gameplay_manager::wave_definition gameplay_manager::m_current_wave_definition;
engine::ref<engine::audio_manager> gameplay_manager::m_audio_manager;
int gameplay_manager::m_available_blocks = 6;
gameplay_manager::tool gameplay_manager::m_current_tool = tool::block;

void gameplay_manager::init(player* playr,engine::ref<engine::text_manager> text_manager, engine::perspective_camera* camera,
	engine::ref<grid> level_grid,
	engine::ref<engine::audio_manager> audio_manager)
{
	m_player_ptr = playr;
	m_player_spawnpoint = m_player_ptr->object()->position();

	m_camera = camera;
	m_level_grid = level_grid;
	m_audio_manager = audio_manager;

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

	m_tool_display = text_hud_element::create(text_manager, "Tool: Turret, cost 100, owned {} max {}", glm::vec2{ 0.025f,0.05f });
	m_tool_display->set_text_size(0.5f);
	hud_manager::add_element(m_tool_display);
	m_tool_display->hide();
	m_prices["turret"] = 100;

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
		std::string tool_text = "Tool: ";
		if (m_current_tool == tool::turret)
		{
			tool_text += "turret. Cost £100, Owned "+std::to_string(turret_manager::count()) +
				", Max " + std::to_string(m_max_turrets);

		}
		else if (m_current_tool == tool::block){
			tool_text += "block. Available " + std::to_string(m_available_blocks);
		}
		m_tool_display->set_text(tool_text);

		int time_remaining = build_time();
		if (time_remaining > 0)
		{
			m_top_display->set_text("Time: "+ std::to_string(time_remaining));
		}
		else {			
			start_combat_phase();
		}
	}

}

void gameplay_manager::damage_portal()
{
	m_portal_health -= 10;
	m_audio_manager->play("alert");
	sfx_manager::jitter_effect.activate(0.2f, 0.5f);	
}

void gameplay_manager::damange_player()
{
	m_health -= 10;
	sfx_manager::cross_fade_effect.activate(1.f);
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
		m_tool_display->show();
		m_wave_active = false;
		m_available_blocks += 4;//TODO make number of blocks awarded determined by wave definition?
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
			e.handled = true;
			break;
		case engine::key_codes::KEY_1:
			m_current_tool = tool::block;
			e.handled = true;
			break;
		case engine::key_codes::KEY_2:
			m_current_tool = tool::turret;
			e.handled = true;
			break;
		//case engine::key_codes::KEY_P:
		//	damage_portal();//DEBUG
		//	break;
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
		//Delete Block
		auto fv = m_camera->front_vector();
		if (fv.y > 0)
		{
			//Not looking at the ground
			return;
		}
		auto cam_pos = m_camera->position();
		auto delta_y = m_level_grid->floor_level() - cam_pos.y;
		if (m_current_tool == tool::turret)
		{
			delta_y += m_level_grid->block_height();//Add height of block.
		}
		auto ground_pos = cam_pos + (delta_y / fv.y) * fv;
		auto grid_coords = m_level_grid->world_to_grid_coords(ground_pos);

		if (m_current_tool == tool::block)
		{
			remove_block(grid_coords.first, grid_coords.second);
		}
		else if (m_current_tool == tool::turret) {
			remove_turret(grid_coords.first, grid_coords.second);
		}
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
		if (m_available_blocks <= 0)
		{
			return;
		}
		auto fv = m_camera->front_vector();
		if (fv.y > 0)
		{
			//Not looking at the ground
			return;
		}
		auto cam_pos = m_camera->position();
		auto delta_y = m_level_grid->floor_level() - cam_pos.y;
		if (m_current_tool == tool::turret)
		{
			delta_y += m_level_grid->block_height();//Add height of block.
		}
		auto ground_pos = cam_pos + (delta_y / fv.y) * fv;
		auto grid_coords = m_level_grid->world_to_grid_coords(ground_pos);

		if (m_current_tool == tool::block)
		{
			place_block(grid_coords.first, grid_coords.second);
		}
		else if (m_current_tool == tool::turret) {
			place_turret(grid_coords.first, grid_coords.second);
		}
	}
}

void gameplay_manager::start_combat_phase()
{
	m_build_timer.reset();
	m_wave_active = true;
	m_tool_display->hide();

	enemy_manager::begin_wave(m_current_wave_definition.num_enemies, m_current_wave_definition.enemy_spacing);	
}

void gameplay_manager::place_block(int x, int z)
{
	bool succeeded = m_level_grid->place_block(x, z);

	if (succeeded)
	{
		if (pathfinder::find_path(*m_level_grid).empty())
		{
			//Placing block blocked the path, so undo
			m_level_grid->remove_block(x, z);
			m_audio_manager->play("error");
		}
		else {
			--m_available_blocks;
		}
	}
}

void gameplay_manager::remove_block(int x, int z)
{
	bool succeeded = m_level_grid->remove_block(x, z);
	if (succeeded)
	{
		++m_available_blocks;
	}	
}

void gameplay_manager::place_turret(int x, int z)
{
	bool success = false;
	if (m_money >= m_prices["turret"])
	{
		auto pair = std::make_pair(x, z);
		if (m_level_grid->is_block(pair)) {
			auto coords = m_level_grid->center_of(x, z);
			coords.y += m_level_grid->block_height();
			success = turret_manager::place_turret(coords);
		}
	}

	if (success)
	{
		m_money -= m_prices["turret"];		
	}
	else {
		m_audio_manager->play("error");
	}
}

void gameplay_manager::remove_turret(int x, int z)
{
	bool success = false;
	
	auto pair = std::make_pair(x, z);
	if (m_level_grid->is_block(pair)) {
		auto coords = m_level_grid->center_of(x, z);
		coords.y += m_level_grid->block_height();
		success = turret_manager::remove_turret(coords);
	}	

	if (success)
	{
		m_money += m_prices["turret"];
	}
	else {
		m_audio_manager->play("error");
	}
}

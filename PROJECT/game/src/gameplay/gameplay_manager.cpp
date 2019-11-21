#include "pch.h"
#include "gameplay_manager.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"
#include "../ai/turret_manager.h"
#include "../sfx/sfx_manager.h"
#include "weapon_manager.h"
#include "pickup_manager.h"
#include "projectile_manager.h"//TESTING

//Static initializers
int gameplay_manager::m_score = 0;
int gameplay_manager::m_money = 100;
int gameplay_manager::m_portal_health = 100;
float gameplay_manager::m_score_multiplier = 1.f;
engine::timer gameplay_manager::m_build_timer{};
int gameplay_manager::m_max_build_time = 30;
std::map<std::string, int> gameplay_manager::m_prices{};
player* gameplay_manager::m_player_ptr;

//HUD elements
engine::ref<text_hud_element> gameplay_manager::m_top_display{};
engine::ref<text_hud_element> gameplay_manager::m_score_display{};
engine::ref<text_hud_element> gameplay_manager::m_money_display{};
engine::ref<text_hud_element> gameplay_manager::m_health_display{};
engine::ref<text_hud_element> gameplay_manager::m_portal_health_display{};
engine::ref<text_hud_element> gameplay_manager::m_tool_display{};
engine::ref<text_hud_element> gameplay_manager::m_message_display{};
engine::ref<text_hud_element> gameplay_manager::m_weapon_charge_display{};

bool gameplay_manager::m_wave_active = false;
std::deque<wave_definition> gameplay_manager::m_waves{};
int gameplay_manager::m_max_waves = 0;
int gameplay_manager::m_wave_number = 0;
engine::perspective_camera* gameplay_manager::m_camera;
engine::ref<grid> gameplay_manager::m_level_grid;
wave_definition gameplay_manager::m_current_wave_definition;
engine::ref<engine::audio_manager> gameplay_manager::m_audio_manager;
int gameplay_manager::m_available_blocks = 6;
bool gameplay_manager::m_fire_weapon = false;
gameplay_manager::tool gameplay_manager::m_current_tool = tool::block;
engine::timer gameplay_manager::m_immunity_timer{};
interactable gameplay_manager::m_hard_mode_switch{};
bool gameplay_manager::m_hardmode_active;

void gameplay_manager::init(player* playr,engine::ref<engine::text_manager> text_manager, engine::perspective_camera* camera,
	engine::ref<grid> level_grid,
	engine::ref<engine::audio_manager> audio_manager,
	std::vector<engine::ref<engine::game_object>>* decorational_objects)
{
	m_player_ptr = playr;

	m_camera = camera;
	m_level_grid = level_grid;
	m_audio_manager = audio_manager;

	engine::ref <engine::model> model = engine::model::create("assets/models/static/dungeon/Lever_Floor.obj");
	engine::game_object_properties props;
	props.is_static = true;
	props.type = 0;
	props.meshes = model->meshes();
	props.textures = model->textures();
	float scale = 1.f/glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f;
	props.position = level_grid->center_of(5, 5);
	auto obj = engine::game_object::create(props);
	decorational_objects->push_back(obj);//Adding the switch model to the game layer's decorational objects for rendering.
	m_hard_mode_switch = interactable(obj,"Press E to activate hard mode");
	m_level_grid->set_state(5, 5, grid_tile::tile_state::border);//Prevent blocks being placed on top of the switch

	//TODO could store the crosshair somewhere? Only if I ever need to modify it.
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

	m_message_display = text_hud_element::create(text_manager, "", glm::vec2{ 0.4f,0.6f });
	m_message_display->set_text_size(0.5f);
	hud_manager::add_element(m_message_display);

	m_weapon_charge_display = text_hud_element::create(text_manager, "", glm::vec2{ 0.75f,0.05f });
	m_weapon_charge_display->set_text_size(1.f);
	m_weapon_charge_display->set_colour(glm::vec4(0.8f,.1f,.1f,1.f));
	hud_manager::add_element(m_weapon_charge_display);

	//Utility function to compute total enemies in wave so that it doesn't have to be recomputed in other places.
	auto total_enemies = [](std::deque<std::pair<int, enemy_type>>& wave_enemies) {
		int count = 0;
		for (auto& item : wave_enemies) {
			count += item.first;
		}
		return count;
	};

	//Adding waves
	std::deque<std::pair<int, enemy_type>> wave_enemies;
	wave_enemies = { {5,enemy_type::robot1},{1,enemy_type::animated_humanoid} };//1st wave
	m_waves.push_back({ total_enemies(wave_enemies),5,30, wave_enemies});

	wave_enemies = { {5,enemy_type::robot1},{2,enemy_type::flyer1} };
	m_waves.push_back({ total_enemies(wave_enemies),3,30, wave_enemies});

	wave_enemies = { {2,enemy_type::robot1},{2,enemy_type::flyer1},{2,enemy_type::robot1},{1,enemy_type::robot2} };
	m_waves.push_back({ total_enemies(wave_enemies),3,30,wave_enemies });

	wave_enemies = { {2,enemy_type::animated_humanoid},{2,enemy_type::robot1},{1,enemy_type::robot2},{2,enemy_type::flyer2} };
	m_waves.push_back({ total_enemies(wave_enemies),2,30, wave_enemies });

	wave_enemies = { {2,enemy_type::animated_humanoid},{3,enemy_type::robot1},{2,enemy_type::robot2},{2,enemy_type::flyer2},{2,enemy_type::flyer1} };//Last wave
	m_waves.push_back({ total_enemies(wave_enemies),1,30,wave_enemies });

	m_max_waves = (int)m_waves.size();

	m_immunity_timer.start();
}

void gameplay_manager::update(const engine::timestep& ts)
{
	check_player_in_bounds();

	m_money_display->set_text("Money: "+std::to_string(m_money));
	m_health_display->set_text("Health: " + std::to_string(health()));
	m_score_display->set_text("Score: "+ std::to_string(m_score));
	m_portal_health_display->set_text("Portal Health: " + std::to_string(m_portal_health));

	//Change the lighting based on current difficulty setting
	if (m_hardmode_active)
	{
		light_manager::set_point_light_colour(glm::vec3(.75f, .1f, .1f));
	}
	else {
		light_manager::set_point_light_colour(light_manager::default_light_colour);
	}

	if (m_wave_active)
	{
		if (m_fire_weapon)
		{
			auto launch_position = m_player_ptr->object()->position();
			launch_position.x += m_camera->front_vector().x;
			launch_position.z += m_camera->front_vector().z;			
			weapon_manager::launch_charged(launch_position, m_camera->front_vector(),ts);
			m_fire_weapon = false;
		}

		weapon_manager::update(ts);

		check_enemies_touching_player();

		//Update wave-only HUD
		auto total_enemies_this_wave = m_current_wave_definition.num_enemies;
		auto spawned_so_far = total_enemies_this_wave - enemy_manager::remaining();
		auto alive = enemy_manager::current_active();
		m_top_display->set_text("Wave: " + std::to_string(m_wave_number) + " of " + std::to_string(m_max_waves)
			+ " | Enemies: " + std::to_string(spawned_so_far) + " of " +
			std::to_string(total_enemies_this_wave) + " alive: " +std::to_string(alive)
		);

		int num_bars = 20 * weapon_manager::charge_percentage();
		m_weapon_charge_display->set_text(std::string(num_bars,'|'));
	}
	else {
		//The player may wish to press the hardmode switch during a build phase
		check_interaction_with_hardmode_switch();

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

void gameplay_manager::damage_player(float amnt)
{
	//The player is immune to damage for short time after taking damage, to prevent death in just a few frames
	if (m_immunity_timer.total() > m_immunity_duration)
	{
		m_immunity_timer.reset();

		m_player_ptr->deal_damage(amnt);
		sfx_manager::cross_fade_effect.activate(1.f);
		sfx_manager::jitter_effect.activate(0.2f, 0.5f);
		if (health() <= 0)
		{
			kill_player();
		}
	}

}

//Begins the next build phase, which will trigger the next wave when it finishes
void gameplay_manager::next_build_phase()
{
	if (!m_waves.empty())
	{
		auto wave_config = m_waves.front();
		m_waves.pop_front();
		m_current_wave_definition = wave_config;
		m_max_build_time = wave_config.prep_time;

		//Toggle relevant HUD
		m_build_timer.start();
		m_tool_display->show();
		m_weapon_charge_display->hide();

		m_wave_active = false;
		m_hardmode_active = false;//Player has to press the switch again if they still want hardmode
		m_score_multiplier = 1.f;//Reset global score multiplier

		//TODO make number of blocks and money awarded determined by wave definition?
		m_available_blocks += 4;
		m_money += 50;

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
			mouse1_event_handler(true);
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

	//Listen for release event for weapon charge/release
	if (!event.handled && event.event_type() == engine::event_type_e::mouse_button_released)
	{
		auto& e = dynamic_cast<engine::mouse_button_released_event&>(event);
		if (e.mouse_button()== engine::mouse_button_codes::MOUSE_BUTTON_1)
		{
			mouse1_event_handler(false);
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
		case engine::key_codes::KEY_T:
			pickup_manager::make_powerup_at(m_player_ptr->object()->position()+glm::vec3(2,0,2));
			break;
		//case engine::key_codes::KEY_P:
		//	damage_portal();//DEBUG
		//	break;
		/*case engine::key_codes::KEY_E:
			sfx_manager::explode_at(m_player_ptr->object()->position());
			sfx_manager::explode_at(m_player_ptr->object()->position()+ glm::vec3(2,0,2));*/
		case engine::key_codes::KEY_G:
			projectile_manager::launch_projectile(false, m_player_ptr->object()->position() + glm::vec3(.5f, 0, 0),glm::vec3(1,0,0));

		default:
			break;
		}
	}
}

/*
Handles mouse button 1 events, with a boolean flag to indicate whether the event is a press,
or a release.
*/
void gameplay_manager::mouse1_event_handler(bool press)
{
	if (!m_wave_active)
	{
		//Remove a block at the targeted position (if possible)
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
		if (press)
		{
			weapon_manager::start_charging();
		}
		else {
			m_fire_weapon = true;
		}
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
	m_message_display->hide();
	m_weapon_charge_display->show();
	if (m_hardmode_active)
	{
		//Hardmode makes enemies come twice as fast
		m_current_wave_definition.enemy_spacing /= 2.f;
		m_score_multiplier = 1.5f;//Extra points in hardmode
	}

	enemy_manager::begin_wave(m_current_wave_definition);	
}

void gameplay_manager::place_block(int x, int z)
{
	if (m_available_blocks <= 0)
	{
		return;
	}

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

//Kill and respawn the player
void gameplay_manager::kill_player()
{
	add_score(-200);//Reduce the score as a penalty for dying
	m_player_ptr->set_health(m_player_ptr->max_health());
	m_player_ptr->object()->set_position(m_player_ptr->spawnpoint());
}

//Kill and respawn the player if they have fallen off the world
void gameplay_manager::check_player_in_bounds()
{
	if (m_player_ptr->object()->position().y < -20.f)
	{
		kill_player();
	}
}

/*
Checks whether any enemies are touching the player, and deals damage to the player if they are
*/
void gameplay_manager::check_enemies_touching_player()
{
	auto& active_enemies = enemy_manager::get_active_enemies();
	auto& player_trigger_box = m_player_ptr->get_trigger_box();
	for (auto& enemy_ptr : active_enemies)
	{
		if (enemy_ptr->get_trigger_box().collision(player_trigger_box))
		{
			damage_player();
		}
	}
}

void gameplay_manager::check_interaction_with_hardmode_switch()
{
	if (!m_hardmode_active && m_hard_mode_switch.in_range_of(m_player_ptr->get_trigger_box()))
	{
		m_message_display->set_text(m_hard_mode_switch.message());
		m_message_display->show();
		if (engine::input::key_pressed(engine::key_codes::KEY_E))
		{
			m_hardmode_active = true;
		}
	}
	else {
		m_message_display->hide();
	}
}

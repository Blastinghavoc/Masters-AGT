#include "pch.h"
#include "flyer_ai.h"
#include "../gameplay/projectile_manager.h"
#include "enemy_manager.h"
#include "../gameplay/gameplay_manager.h"
#include <glm/gtx/norm.hpp>
#include "../entities/static_flying_enemy.h"

std::vector<engine::ref<static_flying_enemy>> flyer_ai::m_active_flyers_this_tick;

/*
Called by the enemy manager once per update, to obtain information that will be needed
for all flyer_ai instances.
*/
void flyer_ai::update_shared_data()
{
	m_active_flyers_this_tick.clear();//Reset

	auto& active_enemies = enemy_manager::get_active_enemies();
	for (auto& enemy: active_enemies)
	{
		if (enemy->type() == enemy_type::flyer1 || enemy->type() == enemy_type::flyer2)
		{
			m_active_flyers_this_tick.push_back(std::dynamic_pointer_cast<static_flying_enemy>(enemy));
		}
	}

}

flyer_ai::flyer_ai()
{
	m_shooting_cooldown_timer.start();
	m_decision_timer.start();
}

//Handle transitions between states
void flyer_ai::update_state(static_flying_enemy& body)
{	
	auto playr = gameplay_manager::get_player();
	auto player_position = playr->object()->position();

	auto adjusted_position = body.position();
	adjusted_position.y = player_position.y;
	//For the purposes of agro, the player and flyer are considered to be on the same level
	float distance2_to_player = glm::distance2(player_position, adjusted_position);

	//Have to compute force, just in case the state we're in needs it for a transition
	m_flock_force = compute_flock_forces(body);
	
	switch (m_state)
	{
	case flyer_state::following_path:
		if (!ready_to_change_state())
		{
			break;
		}

		if (distance2_to_player <= m_agro_range2)
		{
			change_state(flyer_state::shooting);
		}
		//else if (m_flock_force != glm::zero<glm::vec3>()) {
		//	/*
		//	This transition prevents flyers from stacking up at all, even when not shooting.
		//	Personally not sure whether it actually looks better or not.
		//	*/
		//	change_state(flyer_state::making_room,0.f);
		//}
		break;
	case flyer_state::shooting:
		if (!ready_to_change_state())
		{
			break;
		}

		if (distance2_to_player > m_agro_range2)
		{
			int chance_to_pursue = 50;//Percentage chance of pursuing a player who is now out of range

			if (m_previous_state == flyer_state::pursuing)
			{//If we previously chased them, higher chance of continuing to do so (to be extra annoying!)
				chance_to_pursue = 80;
			}

			auto dice_roll = rand() % 100;
			if (dice_roll < chance_to_pursue)
			{
				change_state(flyer_state::pursuing);
			}
			else {
				change_state(flyer_state::returning_to_path);
			}

		}
		else if(m_flock_force != glm::zero<glm::vec3>()) {
			change_state(flyer_state::making_room);
		}
		break;
	case flyer_state::making_room:
		if (!ready_to_change_state())
		{
			break;
		}

		if (m_flock_force == glm::zero<glm::vec3>()) {
			change_state(m_previous_state);
		}
		break;
	case flyer_state::returning_to_path:
		//Always ready for this change
		change_state(flyer_state::following_path);//returning_to_path state is automatically transitioned out of after one update
		break;
	case flyer_state::pursuing:
		if (!ready_to_change_state())
		{
			break;
		}

		change_state(flyer_state::shooting);//pursuing always transitions back to shooting when possible
		break;
	default:
		break;
	}
}

void flyer_ai::on_update(const engine::timestep& time_step, static_flying_enemy& body)
{
	if (body.m_frozen)
	{
		return;//can't do anything if frozen!
	}

	update_state(body);

	//Behaviours based on state
	switch (m_state)
	{
	case flyer_state::following_path:
		follow_path(time_step, body);
		break;
	case flyer_state::shooting:
		shoot(body);
		break;
	case flyer_state::making_room:
		make_room(time_step, body);
		break;
	case flyer_state::returning_to_path:
		return_to_path(body);
		break;
	case flyer_state::pursuing:
		pursue(time_step,body);
		break;
	default:
		break;
	}	
}

//The usual path following behaviour that other enemies do
void flyer_ai::follow_path(const engine::timestep& time_step, static_flying_enemy& body)
{
	body.update_waypoints();

	if (!body.m_waypoints.empty())
	{
		//Try to go towards the first waypoint that we aren't already at
		auto target = body.m_waypoints.front();
		target.y += body.m_y_offset;//Account for the fact that this object is flying

		auto line = target - body.m_object->position();
		auto direction = glm::normalize(line);

		body.move(direction, body.m_movement_speed, time_step, glm::length(line));

	}
}

//Aim at the player and fire a projectile
void flyer_ai::shoot(static_flying_enemy& body)
{
	auto playr = gameplay_manager::get_player();
	auto direction = glm::normalize( playr->object()->position() - body.position());

	float yaw = atan2(direction.x, direction.z);
	//Rotate to face
	body.m_object->set_rotation_axis({ 0,1,0 });
	body.m_object->set_rotation_amount(yaw);

	if (m_shooting_cooldown_timer.total() > m_shooting_cooldown)
	{
		//Shoot if possible
		projectile_manager::launch_projectile(true, body.position(), direction);
		m_shooting_cooldown_timer.reset();
	}
}

/*
Find the closest point on the path to pick up from, which is NOT
necessarily the next point, because the body may have moved.
*/
void flyer_ai::return_to_path(static_flying_enemy& body)
{
	auto& my_position = body.position();
	glm::vec3 closest = *begin(body.m_waypoints);
	float shortest_dist2 = glm::distance2(my_position, closest);
	for (auto it = begin(body.m_waypoints); it != end(body.m_waypoints); ++it) {
		auto tmp_dist = glm::distance2(my_position, *it);
		if (tmp_dist < shortest_dist2)
		{
			shortest_dist2 = tmp_dist;
			closest = *it;
		}
	}

	while (body.m_waypoints.front() != closest)
	{
		body.m_waypoints.pop_front();
	}

	//Next update the flyer will go to the closest position and continue following the path

}

/*
Moves the body in the resultant direction of all the flock forces acting on it
*/
void flyer_ai::make_room(const engine::timestep& time_step,static_flying_enemy& body)
{
	auto length = glm::length(m_flock_force);
	if (length == 0)
	{//May occur if in the make_room state but not enough time has passed to exit that state yet, despite being at correct separation.
		return;
	}
	auto direction = m_flock_force * (1/length);

	if (m_previous_state == flyer_state::shooting)
	{
		//Acting as if the player and flyer are on the same level, to preserve flyer altitude
		auto player_direction = glm::normalize(lateral_vector_to_player(body));
		
		direction = .9f * direction + .1f * player_direction;
		/*Add 10% component in direction of player if previous state was shooting.
		Biases movement to still try to be in range of player.
		*/
	}

	/*Move in direction without limiting distance (so we get a little extra clearance), and without facing the direction of movement.
	This gives a nice "hover out the way" effect instead of the jittery movement obtained if we always face where we're going for
	small corrective movements.
	*/
	body.move(direction, body.m_movement_speed, time_step,INFINITY,false);
}


/*
Move towards the player, aiming to be directly on top of them.
*/
void flyer_ai::pursue(const engine::timestep& time_step, static_flying_enemy& body)
{
	auto line = lateral_vector_to_player(body);
	auto length = glm::length(line);
	if (length == 0)
	{
		return;
	}
	auto player_direction = line * (1/length);

	body.move(player_direction,body.m_movement_speed,time_step,length);
}

/*
Compute the influence of all other flyers on this one.
Anything further away than the desired flock separation is disregarded,
anything closer applies a force.
*/
glm::vec3 flyer_ai::compute_flock_forces(static_flying_enemy& body)
{
	auto& my_position = body.position();
	glm::vec3 force(0);
	for (auto& other : m_active_flyers_this_tick) {
		if (other->id() == body.m_id)
		{
			continue;//skip self
		}
		auto other_pos = other->position();
		auto separation2 = glm::distance2(other_pos, my_position);
		if (separation2 < m_flock_sepparation2)
		{
			if (separation2 != 0) {
				force += (my_position - other_pos) / separation2;//Magnitude of force will be proportional to 1/separation, so really close things push really hard
			}
			else {
				force += glm::vec3(0, 1, 0);//In the absence of any better ideas, go up
			}
		}
	}

	return force;
}

//Compute a vector towards the player, maintaining altitude
glm::vec3 flyer_ai::lateral_vector_to_player(static_flying_enemy& body)
{
	auto playr = gameplay_manager::get_player();
	auto player_position = playr->object()->position();

	auto adjusted_position = body.position();
	adjusted_position.y = player_position.y;

	//Again acting as if the player and flyer are on the same level, to preserve flyer altitude
	auto player_vector = player_position - adjusted_position;
	return player_vector;
}

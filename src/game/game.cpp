/*
** TimeCapsule: A videogame engine.
** Copyright (C) 2026 Rivest Osz
** 
** This file is part of "TimeCapsule".
** 
** "TimeCapsule" is free software: you can redistribute it and/or modify it under the terms of
** the GNU General Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
** 
** "TimeCapsule" is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE. See the GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License along with
** "TimeCapsule". If not, see <https://www.gnu.org/licenses/>. 
**/

#include "game.hpp"
#include "core.hpp"
#include "../engine/asset.hpp"
#include "../engine/gl.hpp"
#include "../engine/math.hpp"
#include "../engine/core.hpp"

namespace timecapsule {
	namespace game {
		int game_sub_init(void) {
			timecapsule::gl_model_t model_person = timecapsule::load_model("resources/models/not-for-distribution/person/scene.gltf");
			if(timecapsule::is_model_valid(model_person) == false)
				return -1;

			{
				timecapsule::model_t &model_person_at_scene = get_current_scene().add_model("person", model_person);
				model_person_at_scene.position.x = 0.f;
			}

			get_current_scene().add_model("a", model_person).position.x = 1.f;
			get_current_scene().add_model("b", model_person).position.x = -1.f;
			get_current_scene().add_model("c", model_person).position.z = 1.f;
			get_current_scene().add_model("d", model_person).position.z = -1.f;

			return 0;
		};

		float time = 0.f;
		float time_since_movement = 0.f;

		void game_sub_update(double dt) {
			if(get_game_state() != GAME_STATE_RUNNING)
				return;
			
			const float speed_crouching = 0.8f;
			const float speed_walking = 1.4f;
			const float speed_running = 4.4f;

			float forward_movement_speed = speed_running;
			float upward_movement_speed = 4.f;

			if(timecapsule::is_key_down(timecapsule::KEY_LEFT_SHIFT)) {
				forward_movement_speed *= 2.f;
				upward_movement_speed *= 4.f;
			}

			if(timecapsule::is_key_down(timecapsule::KEY_LEFT_CONTROL)) {
				forward_movement_speed = speed_crouching;
				upward_movement_speed *= 0.1f;
			}
			
			//if(timecapsule::is_key_pressed(timecapsule::KEY_P))
			//	is_camera_third_person = is_camera_third_person == false;

			timecapsule::camera_t &player_camera = timecapsule::get_player_camera();

			if(is_game_handling_inputs()) {
				timecapsule::update_camera(
					player_camera.goal,
					timecapsule::vector3_t {
						timecapsule::is_key_down(timecapsule::KEY_W) * forward_movement_speed * (float)dt
						- timecapsule::is_key_down(timecapsule::KEY_S) * forward_movement_speed * (float)dt,
						timecapsule::is_key_down(timecapsule::KEY_D) * forward_movement_speed * (float)dt
						- timecapsule::is_key_down(timecapsule::KEY_A) * forward_movement_speed * (float)dt,
						timecapsule::get_mouse_wheel_delta() * upward_movement_speed * (float)dt,
					},
					timecapsule::vector3_t {
						timecapsule::get_mouse_delta().x * M_PI2f * (float)dt,
						timecapsule::get_mouse_delta().y * M_PI2f * (float)dt,
						0.f,
					},
					0.f
				);

				bool is_moving = vdistance(player_camera.current.position, player_camera.goal.position) > 0.f;

				if(is_moving) {
					time_since_movement += dt;
					float interp_time = time_since_movement;

					interp_time = clampf(interp_time, 0.f, 1.f);
					interp_time *= 6.f;
					interp_time = clampf(interp_time, 0.f, 1.f);

					player_camera.current.position = lerp(player_camera.current.position, player_camera.goal.position, interp_time);
					player_camera.current.target = lerp(player_camera.current.target, player_camera.goal.target, interp_time);;
				} else {
					time_since_movement = 0.f;
					player_camera.current.target = player_camera.goal.target;
				}
			} else {
				// [TODO] : add third person camera.
			}

			time += dt;

			/*
			timecapsule::model_t &model_person_at_scene = get_current_scene().get_model("person");
			model_person_at_scene.position.x = std::sin(time);

			if(time >= M_PI2f) {
				model_person_at_scene.rotation.z = std::sin(time);
			}
			*/
		};
	};
};

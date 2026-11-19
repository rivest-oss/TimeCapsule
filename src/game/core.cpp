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

#include "core.hpp"
#include "game.hpp"
#include "../engine/gl.hpp"

namespace timecapsule {
	namespace game {
		game_state_t game_state = GAME_STATE_INVALID;
		bool _game_is_handling_inputs = false;

		void should_game_handle_inputs(bool enable) {
			_game_is_handling_inputs = enable;
		};

		bool is_game_handling_inputs(void) {
			return _game_is_handling_inputs;
		};

		void transition_game_state_to(game_state_t new_state) {
			if(new_state == GAME_STATE_EXIT) {
				game_state = GAME_STATE_EXIT;
				return;
			}
			
			if(game_state == new_state)
				return;
			
			if(new_state == GAME_STATE_RUNNING) {
				timecapsule::disable_cursor();
			}
			
			if(new_state == GAME_STATE_PAUSED) {
				timecapsule::enable_cursor();
			}
			
			game_state = new_state;
		};

		bool is_game_state_exit(void) {
			return game_state == GAME_STATE_EXIT;
		};

		game_state_t get_game_state(void) {
			return game_state;
		};

		int init(void) {
			if(game_sub_init() < 0)
				return -1;
			
			_game_is_handling_inputs = true;
			transition_game_state_to(GAME_STATE_PAUSED);

			return 0;
		};

		int update(void) {
			if(timecapsule::window_should_close()) {
				transition_game_state_to(GAME_STATE_EXIT);
				return -1;
			}
			
			if(is_game_handling_inputs() && (timecapsule::is_key_pressed(timecapsule::KEY_ESCAPE) || timecapsule::is_key_pressed(timecapsule::KEY_PAUSE))) {
				if(game_state == GAME_STATE_RUNNING)
					transition_game_state_to(GAME_STATE_PAUSED);
				
				else if(game_state == GAME_STATE_PAUSED)
					transition_game_state_to(GAME_STATE_RUNNING);
			}
			
			if(is_game_state_exit())
				return -1;
			
			game_sub_update();

			if(is_game_state_exit())
				return -1;

			return 0;
		};

		void deinit(void) {
			// [TODO]
		};
	};
};

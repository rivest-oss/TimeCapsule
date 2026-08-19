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

#pragma once

namespace timecapsule {
	namespace game {
		typedef enum : int {
			GAME_STATE_INVALID = 0,
			GAME_STATE_EXIT,
			GAME_STATE_RUNNING,
			GAME_STATE_PAUSED,
		} game_state_t;
		
		void transition_game_state_to(game_state_t new_state);
		bool is_game_state_exit(void);
		game_state_t get_game_state(void);

		int init(void);
		int update(double dt);
		void deinit(void);
	};
};

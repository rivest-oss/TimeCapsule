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
#include "gl.hpp"

namespace Raylib {
	#include <raylib.h>
	#include <rlgl.h>

	unsigned int rlBeginQuery(void);
	bool rlIsQueryResultReady(unsigned int id);
	double rlGetQueryResultF64(unsigned int id);
	unsigned long rlGetQueryResultI64(unsigned int id);
	void rlClearQueries(void);

	void rlExtrasInit(void);
	void rlExtrasDeinit(void);
};

namespace timecapsule {
	namespace engine_internals {
		void block_game_input(bool enable);
		bool is_key_up(unsigned int key);
		bool is_key_down(unsigned int key);
		bool is_key_pressed(unsigned int key);
		float get_mouse_wheel_delta(void);
		vector2_t get_mouse_delta(void);
	};
};

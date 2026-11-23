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

#define timecapsule_should_render_debug_screen true

namespace timecapsule {
	typedef struct game_render_textures_t {
		Raylib::RenderTexture2D game_play;
		Raylib::RenderTexture2D world_debug;

		static const int dontchange_game_play_downscaled_len = 3;
		Raylib::RenderTexture2D game_play_downscaled[dontchange_game_play_downscaled_len];
		
		void unload(void);
		int resize(int game_w, int game_h, int debug_w, int debug_h);
	} game_render_textures_t;
};

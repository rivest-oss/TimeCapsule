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
#include "scene.hpp"
#include "gl.hpp"

#define timecapsule_should_render_debug_screen true

namespace timecapsule {
	[[noreturn]] void init(int argc, const char **argv);
	void deinit(void);

	void set_current_scene(Scene &scene);
	Scene &get_current_scene(void);
	timecapsule::camera_t &get_player_camera(void);

	double get_time(void);
	double get_unscaled_time(void);
	double get_delta_time(void);
	double get_unscaled_delta_time(void);
	
	double get_time_scale(void);
	void set_time_scale(double factor);
};

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

namespace Raylib {
	#include <raylib.h>
	#include <rlgl.h>
};

namespace timecapsule {
	typedef Raylib::Vector2 vector2_t;
	typedef Raylib::Vector3 vector3_t;
	typedef Raylib::Model gl_model_t;
	typedef Raylib::Camera3D camera_t;

	bool is_model_valid(gl_model_t model);

	enum : unsigned int {
		KEY_W = Raylib::KEY_W,
		KEY_A = Raylib::KEY_A,
		KEY_S = Raylib::KEY_S,
		KEY_D = Raylib::KEY_D,
		KEY_P = Raylib::KEY_P,
		KEY_LEFT_SHIFT = Raylib::KEY_LEFT_SHIFT,
		KEY_LEFT_CONTROL = Raylib::KEY_LEFT_CONTROL,
	};

	bool is_key_up(unsigned int key);
	bool is_key_down(unsigned int key);
	bool is_key_pressed(unsigned int key);

	float get_mouse_wheel_delta(void);
	vector2_t get_mouse_delta(void);

	void update_camera(camera_t &camera, vector3_t movement, vector3_t rotation, float zoom);

	void enable_cursor(void);
	void disable_cursor(void);
	bool is_cursor_enabled(void);

	bool window_should_close(void);
};

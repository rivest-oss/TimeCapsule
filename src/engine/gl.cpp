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

#include "gl.hpp"

namespace timecapsule {
	bool _cursor_enabled = true;
	
	bool is_model_valid(gl_model_t model) {
		return Raylib::IsModelValid(model);
	};
	
	bool is_key_up(unsigned int key) {
		return Raylib::IsKeyUp(key);
	};
	
	bool is_key_down(unsigned int key) {
		return Raylib::IsKeyDown(key);
	};
	
	bool is_key_pressed(unsigned int key) {
		return Raylib::IsKeyPressed(key);
	};
	
	float get_mouse_wheel_delta(void) {
		return Raylib::GetMouseWheelMove();
	};
	
	vector2_t get_mouse_delta(void) {
		return Raylib::GetMouseDelta();
	};
	
	void update_camera(gl_camera_t &camera, vector3_t movement, vector3_t rotation, float zoom) {
		return Raylib::UpdateCameraPro(
			&camera,
			movement,
			rotation,
			zoom
		);
	};
	
	void enable_cursor(void) {
		if(_cursor_enabled == false)
			Raylib::EnableCursor();

		_cursor_enabled = true;
	};
	
	void disable_cursor(void) {
		if(_cursor_enabled)
			Raylib::DisableCursor();
		
		_cursor_enabled = false;
	};
	
	bool is_cursor_enabled(void) {
		return _cursor_enabled;
	};
	
	bool window_should_close(void) {
		return Raylib::WindowShouldClose();
	};
};

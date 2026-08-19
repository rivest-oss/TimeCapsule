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
#include <string>
#include "gl.hpp"

namespace timecapsule {
	int init_resources(void);
	void deinit_resources(void);
	
	timecapsule::gl_image_t load_image(const char *filename);
	timecapsule::gl_texture_t load_texture_from_image(timecapsule::gl_image_t img);
	timecapsule::gl_texture_t load_texture(const char *filename);
	timecapsule::gl_render_texture_t load_render_texture(int width, int height);
	timecapsule::gl_font_t load_font(const char *filename, int font_size = 32, int num_of_characters_to_load = 256);
	timecapsule::gl_model_t load_model(const char *filename);
	timecapsule::gl_model_t load_model_from_mesh(timecapsule::gl_mesh_t mesh);
	
	timecapsule::gl_model_t update_model_diffuse_texture(timecapsule::gl_model_t model, timecapsule::gl_texture_t updated_diffuse_texture);
	
	void unload_image(timecapsule::gl_image_t img);
	void unload_texture(timecapsule::gl_texture_t tex);
	void unload_render_texture(timecapsule::gl_render_texture_t tex);
	void unload_font(timecapsule::gl_font_t font);
	void unload_model(timecapsule::gl_model_t model);
};

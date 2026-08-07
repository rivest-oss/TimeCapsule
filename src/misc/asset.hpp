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
};

namespace timecapsule {
	int init_resources(void);
	void deinit_resources(void);
	
	Raylib::Image load_image(const char *filename);
	Raylib::Texture2D load_texture_from_image(Raylib::Image img);
	Raylib::Texture2D load_texture(const char *filename);
	Raylib::RenderTexture2D load_render_texture(int width, int height);
	Raylib::Font load_font(const char *filename, int font_size = 32, int num_of_characters_to_load = 256);
	Raylib::Model load_model(const char *filename);
	Raylib::Model load_model_from_mesh(Raylib::Mesh mesh);

	Raylib::Model update_model_diffuse_texture(Raylib::Model model, Raylib::Texture2D updated_diffuse_texture);
	
	void unload_image(Raylib::Image img);
	void unload_texture(Raylib::Texture2D tex);
	void unload_render_texture(Raylib::RenderTexture2D tex);
	void unload_font(Raylib::Font font);
	void unload_model(Raylib::Model model);
};

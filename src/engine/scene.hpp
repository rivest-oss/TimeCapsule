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
#include <unordered_map>
#include "asset.hpp"
#include "math.hpp"
#include "gl.hpp"

namespace timecapsule {
	typedef struct model_t {
		timecapsule::gl_model_t rmodel;
		timecapsule::vector3_t position;
		timecapsule::vector3_t scale = timecapsule::vector3_t { 1.f, 1.f, 1.f };
		timecapsule::vector3_t rotation = timecapsule::vector3_t { 0.f, 0.f, 0.f };
		timecapsule::srgb_color_t tint = srgb_color_t { 0xff, 0xff, 0xff, 0xff };

		bool visible = true;
		bool culled = false;
	} model_t;
	
	class Scene {
		private:
			std::unordered_map<std::string, model_t> models;
			unsigned int triangles_count = 0;
		
		public:
			Scene(void);
			~Scene(void);

			void deinit(void);

			model_t &add_model(std::string id, timecapsule::gl_model_t model);
			model_t &get_model(std::string id);
			bool model_exists(std::string id);

			void render(timecapsule::camera_t camera, timecapsule::gl_render_texture_t target, bool update_cull = true);
			void render(timecapsule::camera_t camera, bool update_cull = true);

			unsigned int get_sent_triangles_count(void);
	};
};

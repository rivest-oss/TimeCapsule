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
#include "../math.hpp"

namespace Raylib {
	#include <raylib.h>
};

namespace timecapsule {
	typedef struct model_t {
		Raylib::Model rmodel;
		Raylib::Vector3 position;
		Raylib::Vector3 scale = Raylib::Vector3 { 1.f, 1.f, 1.f };
		Raylib::Vector3 rotation = Raylib::Vector3 { 0.f, 0.f, 0.f };
		Raylib::Color tint = Raylib::WHITE;
	} model_t;
	
	class Scene {
		private:
			model_t invalid_model {
				Raylib::Model {
					Raylib::Matrix {},
					0,
					0,
					nullptr,
					nullptr,
					nullptr,
					Raylib::ModelSkeleton {},
					Raylib::ModelAnimPose {},
					nullptr,
				},
				Raylib::Vector3 {},
				Raylib::Vector3 {},
				Raylib::Vector3 {},
				Raylib::BLANK
			};

			std::unordered_map<std::string, model_t> models;
		
		public:
			Scene(void);
			~Scene(void);

			void deinit(void);

			model_t &add_model(std::string id, Raylib::Model model);
			model_t &get_model(std::string id);
			bool model_exists(std::string id);

			void render(Raylib::Camera3D camera, Raylib::RenderTexture target);
			void render(Raylib::Camera3D camera);
	};
};

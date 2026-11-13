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

#include <cstdio>

namespace Raylib {
	#include <raylib.h>
	#include <rlgl.h>
};

#include "lighting.hpp"

namespace timecapsule {
	namespace lighting {
		Raylib::Shader _shader;

		int init(void) {
			_shader = Raylib::LoadShader("./resources/shaders/lighting.vs", "./resources/shaders/lighting.fs");
			if(Raylib::IsShaderValid(_shader) == false)
				return -1;

			return 0;
		};

		void deinit(void) {
			if(Raylib::IsShaderValid(_shader))
				Raylib::UnloadShader(_shader);
		};

		void update(camera_t camera) {
			if(Raylib::IsShaderValid(_shader) == false)
				return;

			/*
			float light_pos[3] {
				camera.current.position.x,
				camera.current.position.y,
				camera.current.position.z,
			};
			
			Raylib::SetShaderValue(
				_shader,
				_shader.locs[Raylib::SHADER_LOC_VECTOR_VIEW],
				camera_pos_vec3,
				Raylib::SHADER_UNIFORM_VEC3
			);

			for(int i = 0; i < Raylib::MAX_LIGHTS; i++)
				Raylib::UpdateLightValues(_shader, _lights[i]);
			*/
		};

		void begin_rendering(void) {
			if(Raylib::IsShaderValid(_shader) == false)
				return;

			Raylib::BeginShaderMode(_shader);

			Raylib::DrawPlane(
				Raylib::Vector3 {
					0.f,
					0.f,
					0.f,
				},
				Raylib::Vector2 {
					10.f,
					10.f,
				},
				Raylib::WHITE
			);
			Raylib::DrawCube(
				Raylib::Vector3 {
					-4.f,
					0.f,
					-4.f,
				},
				2.f,
				4.f,
				2.f,
				Raylib::WHITE
			);
		};

		void update_model_lighting(gl_model_t &model) {
			for(int i = 0; i < model.materialCount; i++)
				model.materials[i].shader = _shader;
		};

		void end_rendering(void) {
			if(Raylib::IsShaderValid(_shader) == false)
				return;

			Raylib::EndShaderMode();
		};
	};
};

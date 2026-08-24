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

	const int MAX_LIGHTS = 4;
	static int lightsCount = 0;

	typedef struct {   
		int type;
		bool enabled;
		Vector3 position;
		Vector3 target;
		Color color;
		float attenuation;
		
		// Shader locations
		int enabledLoc;
		int typeLoc;
		int positionLoc;
		int targetLoc;
		int colorLoc;
		int attenuationLoc;
	} Light;
	
	typedef enum {
		LIGHT_DIRECTIONAL = 0,
		LIGHT_POINT
	} LightType;

	// Send light properties to shader
	// NOTE: Light shader locations should be available 
	void UpdateLightValues(Shader shader, Light light) {
		// Send to shader light enabled state and type
		SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
		SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

		// Send to shader light position values
		float position[3] = { light.position.x, light.position.y, light.position.z };
		SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

		// Send to shader light target position values
		float target[3] = { light.target.x, light.target.y, light.target.z };
		SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

		// Send to shader light color values
		float color[4] = {
			(float)light.color.r/(float)255,
			(float)light.color.g/(float)255,
			(float)light.color.b/(float)255,
			(float)light.color.a/(float)255
		};

		SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
	}

	// Create a light and get shader locations
	Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader) {
		Light light = { 0 };

		if(lightsCount < MAX_LIGHTS) {
			light.enabled = true;
			light.type = type;
			light.position = position;
			light.target = target;
			light.color = color;

			// NOTE: Lighting shader naming must be the provided ones
			light.enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", lightsCount));
			light.typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", lightsCount));
			light.positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", lightsCount));
			light.targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", lightsCount));
			light.colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", lightsCount));

			UpdateLightValues(shader, light);
			
			lightsCount++;
		}

		return light;
	}
};

#include "lighting.hpp"

namespace timecapsule {
	namespace lighting {
		Raylib::Shader _shader;
		Raylib::Light _lights[Raylib::MAX_LIGHTS];

		int init(void) {
			_shader = Raylib::LoadShader("./resources/shaders/lighting.vs", "./resources/shaders/lighting.fs");
			if(Raylib::IsShaderValid(_shader) == false)
				return -1;

			_shader.locs[Raylib::SHADER_LOC_VECTOR_VIEW] = Raylib::GetShaderLocation(_shader, "viewPos");

			int ambient_loc = Raylib::GetShaderLocation(_shader, "ambient");
			float ambient_vec4[4] = {
				0.1f,
				0.1f,
				0.1,
				1.f,
			};

			Raylib::SetShaderValue(
				_shader,
				ambient_loc,
				ambient_vec4,
				Raylib::SHADER_UNIFORM_VEC4
			);

			_lights[0] = Raylib::CreateLight(
				Raylib::LIGHT_POINT,
				Raylib::Vector3 {
					-2.f,
					1.f,
					-2.f,
				},
				Raylib::Vector3 { 0.f, 0.f, 0.f },
				Raylib::YELLOW,
				_shader
			);

			_lights[1] = Raylib::CreateLight(
				Raylib::LIGHT_POINT,
				Raylib::Vector3 {
					2.f,
					1.f,
					2.f,
				},
				Raylib::Vector3 { 0.f, 0.f, 0.f },
				Raylib::RED,
				_shader
			);

			_lights[2] = Raylib::CreateLight(
				Raylib::LIGHT_POINT,
				Raylib::Vector3 {
					-2.f,
					1.f,
					2.f,
				},
				Raylib::Vector3 { 0.f, 0.f, 0.f },
				Raylib::GREEN,
				_shader
			);

			_lights[3] = Raylib::CreateLight(
				Raylib::LIGHT_POINT,
				Raylib::Vector3 {
					2.f,
					1.f,
					-2.f,
				},
				Raylib::Vector3 { 0.f, 0.f, 0.f },
				Raylib::BLUE,
				_shader
			);

			return 0;
		};

		void deinit(void) {
			if(Raylib::IsShaderValid(_shader))
				Raylib::UnloadShader(_shader);
		};

		void update(camera_t camera) {
			if(Raylib::IsShaderValid(_shader) == false)
				return;

			float camera_pos_vec3[3] {
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
					0.f,
					0.f,
					0.f,
				},
				2.f,
				4.f,
				2.f,
				Raylib::WHITE
			);
		};

		void end_rendering(void) {
			if(Raylib::IsShaderValid(_shader) == false)
				return;

			Raylib::EndShaderMode();
			std::printf("\n");

			for(int i = 0; i < Raylib::MAX_LIGHTS; i++) {
				std::printf("%i:\n", i);

				std::printf(
					"    * Position: %.2f, %.2f, %2.f\n",
					_lights[i].position.x,
					_lights[i].position.y,
					_lights[i].position.z
				);
				std::printf(
					"    * Enabled: %s\n",
					_lights[i].enabled ? "True" : "False"
				);
				std::printf(
					"    * Color: %02x%02x%02x(%02x)\n",
					_lights[i].color.r,
					_lights[i].color.g,
					_lights[i].color.b,
					_lights[i].color.a
				);

				if(_lights[i].enabled) {
					Raylib::DrawSphereEx(
						_lights[i].position,
						0.2f,
						8,
						8,
						_lights[i].color
					);
				} else {
					Raylib::DrawSphereWires(
						_lights[i].position,
						0.2f,
						8,
						8,
						_lights[i].color
					);
				}
			};

			std::printf("\n");
		};
	};
};

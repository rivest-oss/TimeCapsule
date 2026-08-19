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
#include <cstdlib>
#include "core.hpp"
#include "math.hpp"
#include "gl.hpp"
#include "asset.hpp"
#include "../misc/rtconfig.hpp"
#include "../game/core.hpp"

namespace timecapsule {
	Raylib::RenderTexture2D _render_texture_main_game_screen;
	Raylib::RenderTexture2D _render_texture_debug_screen;
	timecapsule::camera_t _player_camera;
	timecapsule::camera_t _debug_camera;
	timecapsule::Scene _curr_scene;

	void render(void);
	
	[[noreturn]] void init(int argc, const char **argv) {
		timecapsule::config_t conf = timecapsule::read_config_from_args(argc, argv);
		(void)conf;
		
		Raylib::SetTargetFPS(60);

		Raylib::SetConfigFlags(
			Raylib::FLAG_WINDOW_RESIZABLE
		);
		
		Raylib::InitWindow(640, 360, "TimeCapsule");
		while(Raylib::IsWindowReady() == false) {};
		
		Raylib::SetExitKey(Raylib::KEY_NULL);
		
		Raylib::InitAudioDevice();
		while(Raylib::IsAudioDeviceReady() == false) {};
		
		if(timecapsule::init_resources() < 0) {
			deinit();
			std::exit(1);
		}

		if(timecapsule_should_render_debug_screen) {
			_render_texture_main_game_screen = timecapsule::load_render_texture(16, 16);
			if(Raylib::IsRenderTextureValid(_render_texture_main_game_screen) == false) {
				std::fprintf(stderr, "Couldn't load game main render texture.\n");
				deinit();
				std::exit(1);
			}
			
			_render_texture_debug_screen = timecapsule::load_render_texture(16, 16);
			if(Raylib::IsRenderTextureValid(_render_texture_debug_screen) == false) {
				std::fprintf(stderr, "Couldn't load game main debug render texture.\n");
				deinit();
				std::exit(1);
			}
		}

		_curr_scene.deinit();
		
		_player_camera.position = Raylib::Vector3 { 0.f, 2.f, 4.f };
		_player_camera.target = Raylib::Vector3 { 0.f, 0.f, 0.f };
		_player_camera.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
		_player_camera.fovy = 45.f;
		_player_camera.projection = Raylib::CAMERA_PERSPECTIVE;

		_debug_camera.position = Raylib::Vector3 { 0.f, 2.f, 4.f };
		_debug_camera.target = Raylib::Vector3 { 0.f, 0.f, 0.f };
		_debug_camera.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
		_debug_camera.fovy = 45.f;
		_debug_camera.projection = Raylib::CAMERA_PERSPECTIVE;
		
		if(timecapsule::game::init() < 0) {
			deinit();
			std::exit(1);
		}
		
		while(true) {
			int rc = timecapsule::game::update(Raylib::GetFrameTime());
			if(rc < 0)
				break;

			render();
		};
		
		deinit();
		std::exit(0);
	};
	
	void draw_camera_frustrum_in_3d(Raylib::Camera camera, Raylib::Vector3 camera_sphere_radius_ref) {
		// center
		Raylib::Ray ray_center = Raylib::GetScreenToWorldRayEx(
			Raylib::Vector2 {
				float(_render_texture_main_game_screen.texture.width) / 2.f,
				float(_render_texture_main_game_screen.texture.height) / 2.f,
			},
			camera,
			_render_texture_main_game_screen.texture.width,
			_render_texture_main_game_screen.texture.height
		);
		
		// top left
		Raylib::Ray ray_tl = Raylib::GetScreenToWorldRayEx(
			Raylib::Vector2 {
				0.f,
				0.f,
			},
			camera,
			_render_texture_main_game_screen.texture.width,
			_render_texture_main_game_screen.texture.height
		);
		// top right
		Raylib::Ray ray_tr = Raylib::GetScreenToWorldRayEx(
			Raylib::Vector2 {
				(float)_render_texture_main_game_screen.texture.width - 1,
				0.f,
			},
			camera,
			_render_texture_main_game_screen.texture.width,
			_render_texture_main_game_screen.texture.height
		);
		// bottom left
		Raylib::Ray ray_bl = Raylib::GetScreenToWorldRayEx(
			Raylib::Vector2 {
				0.f,
				(float)_render_texture_main_game_screen.texture.height - 1,
			},
			camera,
			_render_texture_main_game_screen.texture.width,
			_render_texture_main_game_screen.texture.height
		);
		// bottom right
		Raylib::Ray ray_br = Raylib::GetScreenToWorldRayEx(
			Raylib::Vector2 {
				(float)_render_texture_main_game_screen.texture.width - 1,
				(float)_render_texture_main_game_screen.texture.height - 1,
			},
			camera,
			_render_texture_main_game_screen.texture.width,
			_render_texture_main_game_screen.texture.height
		);
		
		float dist_near = float(Raylib::rlGetCullDistanceNear());
		//float dist_far = float(Raylib::rlGetCullDistanceFar());
		float dist_far = dist_near + 0.1f;
		
		// drawing
		
		// (center & corners, 0,0 to dist_far)
		{
			// draw center
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x,
					camera.position.y,
					camera.position.z,
				},
				Raylib::Vector3 {
					camera.position.x + ray_center.direction.x * dist_far,
					camera.position.y + ray_center.direction.y * dist_far,
					camera.position.z + ray_center.direction.z * dist_far,
				},
				Raylib::GREEN
			);
			// draw top left
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x,
					camera.position.y,
					camera.position.z,
				},
				Raylib::Vector3 {
					camera.position.x + ray_tl.direction.x * dist_far,
					camera.position.y + ray_tl.direction.y * dist_far,
					camera.position.z + ray_tl.direction.z * dist_far,
				},
				Raylib::RED
			);
			// draw top right
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x,
					camera.position.y,
					camera.position.z,
				},
				Raylib::Vector3 {
					camera.position.x + ray_tr.direction.x * dist_far,
					camera.position.y + ray_tr.direction.y * dist_far,
					camera.position.z + ray_tr.direction.z * dist_far,
				},
				Raylib::RED
			);
			// draw bottom left
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x,
					camera.position.y,
					camera.position.z,
				},
				Raylib::Vector3 {
					camera.position.x + ray_bl.direction.x * dist_far,
					camera.position.y + ray_bl.direction.y * dist_far,
					camera.position.z + ray_bl.direction.z * dist_far,
				},
				Raylib::RED
			);
			// draw bottom right
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x,
					camera.position.y,
					camera.position.z,
				},
				Raylib::Vector3 {
					camera.position.x + ray_br.direction.x * dist_far,
					camera.position.y + ray_br.direction.y * dist_far,
					camera.position.z + ray_br.direction.z * dist_far,
				},
				Raylib::RED
			);
		}
		
		// (corners, dist_near)
		{
			// TL->TR
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x + ray_tl.direction.x * dist_near,
					camera.position.y + ray_tl.direction.y * dist_near,
					camera.position.z + ray_tl.direction.z * dist_near,
				},
				Raylib::Vector3 {
					camera.position.x + ray_tr.direction.x * dist_near,
					camera.position.y + ray_tr.direction.y * dist_near,
					camera.position.z + ray_tr.direction.z * dist_near,
				},
				Raylib::PURPLE
			);
			// BL->BR
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x + ray_bl.direction.x * dist_near,
					camera.position.y + ray_bl.direction.y * dist_near,
					camera.position.z + ray_bl.direction.z * dist_near,
				},
				Raylib::Vector3 {
					camera.position.x + ray_br.direction.x * dist_near,
					camera.position.y + ray_br.direction.y * dist_near,
					camera.position.z + ray_br.direction.z * dist_near,
				},
				Raylib::PURPLE
			);
			// TL->BL
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x + ray_tl.direction.x * dist_near,
					camera.position.y + ray_tl.direction.y * dist_near,
					camera.position.z + ray_tl.direction.z * dist_near,
				},
				Raylib::Vector3 {
					camera.position.x + ray_bl.direction.x * dist_near,
					camera.position.y + ray_bl.direction.y * dist_near,
					camera.position.z + ray_bl.direction.z * dist_near,
				},
				Raylib::PURPLE
			);
			// TR->BR
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x + ray_tr.direction.x * dist_near,
					camera.position.y + ray_tr.direction.y * dist_near,
					camera.position.z + ray_tr.direction.z * dist_near,
				},
				Raylib::Vector3 {
					camera.position.x + ray_br.direction.x * dist_near,
					camera.position.y + ray_br.direction.y * dist_near,
					camera.position.z + ray_br.direction.z * dist_near,
				},
				Raylib::PURPLE
			);
		}
		
		// (corners, dist_far)
		{
			// TL->TR
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x + ray_tl.direction.x * dist_far,
					camera.position.y + ray_tl.direction.y * dist_far,
					camera.position.z + ray_tl.direction.z * dist_far,
				},
				Raylib::Vector3 {
					camera.position.x + ray_tr.direction.x * dist_far,
					camera.position.y + ray_tr.direction.y * dist_far,
					camera.position.z + ray_tr.direction.z * dist_far,
				},
				Raylib::YELLOW
			);
			// BL->BR
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x + ray_bl.direction.x * dist_far,
					camera.position.y + ray_bl.direction.y * dist_far,
					camera.position.z + ray_bl.direction.z * dist_far,
				},
				Raylib::Vector3 {
					camera.position.x + ray_br.direction.x * dist_far,
					camera.position.y + ray_br.direction.y * dist_far,
					camera.position.z + ray_br.direction.z * dist_far,
				},
				Raylib::YELLOW
			);
			// TL->BL
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x + ray_tl.direction.x * dist_far,
					camera.position.y + ray_tl.direction.y * dist_far,
					camera.position.z + ray_tl.direction.z * dist_far,
				},
				Raylib::Vector3 {
					camera.position.x + ray_bl.direction.x * dist_far,
					camera.position.y + ray_bl.direction.y * dist_far,
					camera.position.z + ray_bl.direction.z * dist_far,
				},
				Raylib::YELLOW
			);
			// TR->BR
			Raylib::DrawLine3D(
				Raylib::Vector3 {
					camera.position.x + ray_tr.direction.x * dist_far,
					camera.position.y + ray_tr.direction.y * dist_far,
					camera.position.z + ray_tr.direction.z * dist_far,
				},
				Raylib::Vector3 {
					camera.position.x + ray_br.direction.x * dist_far,
					camera.position.y + ray_br.direction.y * dist_far,
					camera.position.z + ray_br.direction.z * dist_far,
				},
				Raylib::YELLOW
			);
		}
		
		// draw camera position sphere
		{
			float camera_sphere_radius = vdistance(
				camera.position,
				camera_sphere_radius_ref
			);
			
			if(camera_sphere_radius < 1.f) camera_sphere_radius = 1.f;
			camera_sphere_radius *= 0.001f;
			
			Raylib::DrawSphere(camera.position, camera_sphere_radius, Raylib::WHITE);
		}
	};

	void render(void) {
		Raylib::BeginDrawing();
		Raylib::ClearBackground(Raylib::BLACK);
		
		int screen_width = Raylib::GetScreenWidth();
		int screen_height = Raylib::GetScreenHeight();

		if(screen_width < 2 || screen_height < 2) {
			std::printf("[DEBUG] Screen is %ix%i, rendering aborted.\n", screen_width, screen_height);
			
			if(int(Raylib::GetTime()) & 0x1)
				Raylib::ClearBackground(Raylib::Color { 0xff, 0x00, 0x00, 0xff });
			
			Raylib::EndDrawing();
			return;
		}

		int game_main_screen_width = screen_width * 0.60f;
		int game_main_screen_height = screen_height;
		int game_main_debug_screen_width = screen_width * 0.40f;
		int game_main_debug_screen_height = screen_height;

		if(game_main_screen_width < 16) game_main_screen_width = 16;
		if(game_main_screen_height < 16) game_main_screen_height = 16;

		if(timecapsule_should_render_debug_screen) {
			if(
				game_main_screen_width != _render_texture_main_game_screen.texture.width
				|| game_main_screen_height != _render_texture_main_game_screen.texture.height
			) {
				timecapsule::unload_render_texture(_render_texture_main_game_screen);
				
				_render_texture_main_game_screen = timecapsule::load_render_texture(game_main_screen_width, game_main_screen_height);
				if(Raylib::IsRenderTextureValid(_render_texture_main_game_screen) == false) {
					std::fprintf(stderr, "Couldn't load game main render texture.\n");
					deinit();
					std::exit(1);
				}
			}

			if(
				game_main_debug_screen_width != _render_texture_debug_screen.texture.width
				|| game_main_debug_screen_height != _render_texture_debug_screen.texture.height
			) {
				timecapsule::unload_render_texture(_render_texture_debug_screen);
				
				_render_texture_debug_screen = timecapsule::load_render_texture(game_main_debug_screen_width, game_main_debug_screen_height);
				if(Raylib::IsRenderTextureValid(_render_texture_debug_screen) == false) {
					std::fprintf(stderr, "Couldn't load game main render texture.\n");
					deinit();
					std::exit(1);
				}
			}

			_curr_scene.render(_player_camera, _render_texture_main_game_screen);
			_curr_scene.render(_debug_camera, _render_texture_debug_screen);

			Raylib::BeginTextureMode(_render_texture_debug_screen);
			Raylib::BeginMode3D(_debug_camera);
			
			draw_camera_frustrum_in_3d(_player_camera, _debug_camera.position);

			Raylib::EndMode3D();
			Raylib::EndTextureMode();
		} else {
			_curr_scene.render(_player_camera);
		}

		if(timecapsule_should_render_debug_screen) {
			Raylib::DrawTexturePro(
				_render_texture_main_game_screen.texture,
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_render_texture_main_game_screen.texture.width,
					-(float)_render_texture_main_game_screen.texture.height,
				},
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_render_texture_main_game_screen.texture.width,
					(float)_render_texture_main_game_screen.texture.height,
				},
				Raylib::Vector2 {
					0.f,
					0.f,
				},
				0.f,
				Raylib::WHITE
			);

			Raylib::DrawTexturePro(
				_render_texture_debug_screen.texture,
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_render_texture_debug_screen.texture.width,
					-(float)_render_texture_debug_screen.texture.height,
				},
				Raylib::Rectangle {
					(float)_render_texture_main_game_screen.texture.width,
					0.f,
					(float)_render_texture_debug_screen.texture.width,
					(float)_render_texture_debug_screen.texture.height,
				},
				Raylib::Vector2 {
					0.f,
					0.f,
				},
				0.f,
				Raylib::WHITE
			);
		}

		Raylib::EndDrawing();
	};
	
	void deinit(void) {
		Raylib::EnableCursor();
		
		timecapsule::game::deinit();
		_curr_scene.deinit();
		timecapsule::deinit_resources();
		
		Raylib::CloseAudioDevice();
		Raylib::CloseWindow();
	};

	void set_current_scene(Scene &scene) {
		_curr_scene = scene;
	};

	Scene &get_current_scene(void) {
		return _curr_scene;
	};

	timecapsule::camera_t &get_player_camera(void) {
		return _player_camera;
	};
};

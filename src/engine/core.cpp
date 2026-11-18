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
#include "gl.hpp"
#include "asset.hpp"
#include "../misc/rtconfig.hpp"
#include "../game/core.hpp"
#include "lighting.hpp"
#include "rlgl-extra.hpp"
#include "hl-debug.hpp"

namespace timecapsule {
	typedef struct game_render_textures_t {
		Raylib::RenderTexture2D game_play;
		Raylib::RenderTexture2D world_debug;

		Raylib::RenderTexture2D game_play_downscaled[8];

		void unload(void) {
			if(Raylib::IsRenderTextureValid(game_play))
				unload_render_texture_with_depth(game_play);
			if(Raylib::IsRenderTextureValid(world_debug))
				unload_render_texture(world_debug);
			
			for(int i = 0; i < 8; i++) {
				if(Raylib::IsRenderTextureValid(game_play_downscaled[i]))
					unload_render_texture(game_play_downscaled[i]);

				game_play_downscaled[i].id =
				game_play_downscaled[i].texture.id =
				game_play_downscaled[i].texture.width =
				game_play_downscaled[i].texture.height = 0;
			};

			game_play.id =
			game_play.texture.id =
			game_play.texture.width =
			game_play.texture.height =
			world_debug.id =
			world_debug.texture.id =
			world_debug.texture.width =
			world_debug.texture.height = 0;
		};

		int resize(int game_w, int game_h, int debug_w, int debug_h) {
			int old_game_w = game_play.texture.width;
			int old_game_h = game_play.texture.height;
			int old_debug_w = world_debug.texture.width;
			int old_debug_h = world_debug.texture.height;

			if(
				Raylib::IsRenderTextureValid(game_play) == false
				|| Raylib::IsRenderTextureValid(world_debug) == false
				|| old_game_w != game_w
				|| old_game_h != game_h
				|| old_debug_w != debug_w
				|| old_debug_h != debug_h
			) {
				unload();

				game_play = load_render_texture_with_depth(game_w, game_h);
				world_debug = load_render_texture(debug_w, debug_h);
				
				for(int i = 1; i < 8; i++) {
					if(((float)game_w / i) < 16.f || ((float)game_h / i) < 16.f)
						break;

					game_play_downscaled[i - 1] = load_render_texture(
						(float)game_w / i,
						(float)game_h / i
					);

					if(Raylib::IsRenderTextureValid(game_play_downscaled[i - 1]) == false) {
						unload();
						return -1;
					}

					Raylib::SetTextureFilter(game_play_downscaled[i - 1].texture, Raylib::TEXTURE_FILTER_BILINEAR);
				};

				if(
					Raylib::IsRenderTextureValid(game_play) == false
					|| Raylib::IsRenderTextureValid(world_debug) == false
				) {
					unload();
					return -1;
				}
			}

			return 0;
		};
	} game_render_textures_t;

	unsigned int _debug_screen_page = 0;
	game_render_textures_t _game_render_textures;
	timecapsule::camera_t _player_camera;
	timecapsule::camera_t _debug_camera;
	timecapsule::Scene _curr_scene;
	Raylib::Shader _dof_shader;

	void render(void);
	
	[[noreturn]] void init(int argc, const char **argv) {
		timecapsule::config_t conf = timecapsule::read_config_from_args(argc, argv);
		(void)conf;
		
		Raylib::SetTargetFPS(60);

		Raylib::SetConfigFlags(
			Raylib::FLAG_WINDOW_RESIZABLE
			| Raylib::FLAG_MSAA_4X_HINT
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

		if(timecapsule::lighting::init() < 0) {
			deinit();
			std::exit(1);
		}

		_debug_screen_page = 0u;

		if(_game_render_textures.resize(1, 1, 1, 1) < 0) {
			std::fprintf(stderr, "[ENGINE] Error: Couldn't load render textures\n");
			deinit();
			std::exit(1);
		}

		_curr_scene.deinit();
		
		_player_camera.current.position = _player_camera.goal.position = Raylib::Vector3 { 0.f, 0.6f, 1.f };
		_player_camera.current.target = _player_camera.goal.target = Raylib::Vector3 { 0.3, 0.4, -0.7, };
		_player_camera.current.up = _player_camera.goal.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
		_player_camera.current.fovy = _player_camera.goal.fovy = 45.f;
		_player_camera.current.projection = _player_camera.goal.projection = Raylib::CAMERA_PERSPECTIVE;

		_debug_camera.current.position = _debug_camera.goal.position = Raylib::Vector3 { 2.f, 3.f, 4.f };
		_debug_camera.current.target = _debug_camera.goal.target = Raylib::Vector3 { 0.f, 0.f, 0.f };
		_debug_camera.current.up = _debug_camera.goal.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
		_debug_camera.current.fovy = _debug_camera.goal.fovy = 45.f;
		_debug_camera.current.projection = _debug_camera.goal.projection = Raylib::CAMERA_PERSPECTIVE;

		_dof_shader = Raylib::LoadShader(nullptr, "./resources/shaders/dof.fs");
		if(Raylib::IsShaderValid(_dof_shader) == false) {
			std::fprintf(stderr, "[ENGINE] Error: Couldn't load load or compile dof shader.\n");
			deinit();
			std::exit(1);
		}
		
		if(timecapsule::game::init() < 0) {
			deinit();
			std::exit(1);
		}

		bool is_game_input_admin = true;
		
		while(true) {
			if(timecapsule_should_render_debug_screen) {
				if(is_key_pressed(Raylib::KEY_K))
					is_game_input_admin = is_game_input_admin == false;
				
				timecapsule::game::should_game_handle_inputs(is_game_input_admin);
			}

			int rc = timecapsule::game::update(Raylib::GetFrameTime());
			if(rc < 0)
				break;
			
			render();
		};
		
		deinit();
		std::exit(0);
	};
	
	highlevel_debug::_debug_perf_entry_t _debug_perf_entries[4096];
	
	void render(void) {
		int _debug_perf_entries_len = 0;
		
		#define PERF_START_RECORD Raylib::rlBeginQuery();
		
		#define PERF_STOP_RECORD(s,colorid)	\
			Raylib::rlEndQuery();	\
			_debug_perf_entries[_debug_perf_entries_len++] = highlevel_debug::_debug_perf_entry_t {	\
				s,	\
				Raylib::rlGetQueryResult(),	\
				colorid,	\
			};
		
		Raylib::BeginDrawing();
		
		Raylib::ClearBackground(Raylib::BLACK);

		if(Raylib::IsKeyPressed(Raylib::KEY_F3))
			_debug_screen_page++;

		if(
			timecapsule_should_render_debug_screen == false
			|| (timecapsule_should_render_debug_screen && _debug_screen_page > 1u)
		) {
			_debug_screen_page = 0u;
		}
		
		int screen_width = Raylib::GetScreenWidth();
		int screen_height = Raylib::GetScreenHeight();

		if(screen_width < 2 || screen_height < 2) {
			std::printf("[DEBUG] Screen is %ix%i, rendering aborted.\n", screen_width, screen_height);
			
			if(int(Raylib::GetTime()) & 0x1)
				Raylib::ClearBackground(Raylib::Color { 0xff, 0x00, 0x00, 0xff });
			
			Raylib::EndDrawing();
			return;
		}
		
		PERF_START_RECORD;

		int game_main_screen_width = screen_width * 0.60f;
		int game_main_screen_height = screen_height;
		int game_main_debug_screen_width = screen_width * 0.39f;
		int game_main_debug_screen_height = screen_height;

		if(game_main_screen_width < 16) game_main_screen_width = 16;
		if(game_main_screen_height < 16) game_main_screen_height = 16;

		if(
			_game_render_textures.resize(
				game_main_screen_width,
				game_main_screen_height,
				timecapsule_should_render_debug_screen ? game_main_debug_screen_width : 1,
				timecapsule_should_render_debug_screen ? game_main_debug_screen_height : 1
			) < 0
		) {
			PERF_STOP_RECORD("resolution change", 0);
			
			std::fprintf(stderr, "[ENGINE] Error: Couldn't load render textures\n");
			deinit();
			std::exit(1);
		}
		
		PERF_STOP_RECORD("resolution change", 1);
		PERF_START_RECORD;

		unsigned int player_render_triangle_count;
		unsigned int debug_render_triangle_count = 0;

		_curr_scene.render(_player_camera, _game_render_textures.game_play);
		player_render_triangle_count = _curr_scene.get_sent_triangles_count();
		
		PERF_STOP_RECORD("player render", 2);
		PERF_START_RECORD;

		if(timecapsule_should_render_debug_screen) {
			_curr_scene.render(_debug_camera, _game_render_textures.world_debug);
			debug_render_triangle_count = _curr_scene.get_sent_triangles_count();

			Raylib::BeginTextureMode(_game_render_textures.world_debug);
			Raylib::BeginMode3D(_debug_camera.current);
			
			highlevel_debug::draw_camera_frustrum_in_3d(_player_camera, _debug_camera.current.position, _game_render_textures.game_play.texture);

			Raylib::EndMode3D();
			Raylib::EndTextureMode();
			
			PERF_STOP_RECORD("debug render", 3);
			PERF_START_RECORD;
		}
		
		for(int i = 0; i < 8; i++) {
			Raylib::RenderTexture2D target = _game_render_textures.game_play_downscaled[i];
			Raylib::RenderTexture2D source = i < 1 ? _game_render_textures.game_play : _game_render_textures.game_play_downscaled[i - 1];

			Raylib::BeginTextureMode(target);
				Raylib::DrawTexturePro(
					source.texture,
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)source.texture.width,
						(float)source.texture.height,
					},
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)target.texture.width,
						(float)target.texture.height,
					},
					Raylib::Vector2 {
						0.f,
						0.f,
					},
					0.f,
					Raylib::WHITE
				);
			Raylib::EndTextureMode();
		};
		
		PERF_STOP_RECORD("blurred scene", 4);
		PERF_START_RECORD;

		if(timecapsule_should_render_debug_screen) {
			Raylib::BeginShaderMode(_dof_shader);
				Raylib::Texture2D blurry_texture = _game_render_textures.game_play.texture;
				for(int i = 0; i < 8; i++) {
					if(Raylib::IsRenderTextureValid(_game_render_textures.game_play_downscaled[i]) == false)
						break;

					blurry_texture = _game_render_textures.game_play_downscaled[i].texture;
				};

				Raylib::SetShaderValueTexture(_dof_shader, Raylib::GetShaderLocation(_dof_shader, "depthTexture"), _game_render_textures.game_play.depth);
				Raylib::SetShaderValueTexture(_dof_shader, Raylib::GetShaderLocation(_dof_shader, "focusedTexture"), _game_render_textures.game_play.texture);
				Raylib::SetShaderValueTexture(_dof_shader, Raylib::GetShaderLocation(_dof_shader, "blurryTexture"), blurry_texture);

				Raylib::DrawTexturePro(
					_game_render_textures.game_play.texture,
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play.texture.width,
						-(float)_game_render_textures.game_play.texture.height,
					},
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play.texture.width,
						(float)_game_render_textures.game_play.texture.height,
					},
					Raylib::Vector2 {
						0.f,
						0.f,
					},
					0.f,
					Raylib::WHITE
				);
			Raylib::EndShaderMode();
			
			PERF_STOP_RECORD("postprocessing", 5);
			PERF_START_RECORD;
			
			Raylib::DrawTexturePro(
				_game_render_textures.world_debug.texture,
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_game_render_textures.world_debug.texture.width,
					-(float)_game_render_textures.world_debug.texture.height,
				},
				Raylib::Rectangle {
					(float)screen_width - _game_render_textures.world_debug.texture.width,
					0.f,
					(float)_game_render_textures.world_debug.texture.width,
					(float)_game_render_textures.world_debug.texture.height,
				},
				Raylib::Vector2 {
					0.f,
					0.f,
				},
				0.f,
				Raylib::WHITE
			);

			int border_width = screen_width - (game_main_debug_screen_width + game_main_screen_width);
			if(border_width < 0) border_width *= -1;
			
			Raylib::DrawRectangle(
				_game_render_textures.game_play.texture.width,
				0,
				border_width,
				screen_height,
				Raylib::DARKGRAY
			);
			
			Raylib::DrawRectangle(
				_game_render_textures.game_play.texture.width + border_width / 4.f,
				0,
				border_width / 2.f,
				screen_height,
				Raylib::GRAY
			);
		} else {
			Raylib::DrawTexturePro(
				_game_render_textures.game_play.texture,
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_game_render_textures.game_play.texture.width,
					-(float)_game_render_textures.game_play.texture.height,
				},
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_game_render_textures.game_play.texture.width,
					(float)_game_render_textures.game_play.texture.height,
				},
				Raylib::Vector2 {
					0.f,
					0.f,
				},
				0.f,
				Raylib::WHITE
			);
		}
		
		PERF_STOP_RECORD("screen", 6);

		highlevel_debug::draw_debug_page(_debug_screen_page, highlevel_debug::perf_data_t {
			_debug_perf_entries,
			_debug_perf_entries_len,
			player_render_triangle_count,
			debug_render_triangle_count,
		});
		
		Raylib::EndDrawing();
	};
	
	void deinit(void) {
		Raylib::EnableCursor();
		
		timecapsule::game::deinit();
		_curr_scene.deinit();
		timecapsule::lighting::deinit();
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
	
	bool _what_thingy_should_i_update = false;
	
	timecapsule::camera_t &get_player_camera(void) {
		//return _player_camera;
		if(Raylib::IsKeyPressed(Raylib::KEY_P)) _what_thingy_should_i_update = _what_thingy_should_i_update == false;
		return _what_thingy_should_i_update ? _debug_camera : _player_camera;
	};
};

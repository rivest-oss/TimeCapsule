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

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "core.hpp"
#include "core-private.hpp"
#include "gl.hpp"
#include "asset.hpp"
#include "../misc/rtconfig.hpp"
#include "lighting.hpp"
#include "rlgl-extra.hpp"
#include "hl-debug.hpp"

namespace timecapsule {
	double scaled_time = 0.0;
	double unscaled_time = 0.0;
	double scaled_delta_time = 0.0;
	double unscaled_delta_time = 0.0;
	double time_scale = 1.0;

	bool _recording_perf_enabled = false;
	double _recording_perf_time = 0.0;
	unsigned int _recording_perf_frame_number = 0u;
	double _recording_perf_frame_time = 0.0;

	game_render_textures_t _game_render_textures;
	timecapsule::camera_t _player_camera;
	timecapsule::camera_t _debug_camera;
	timecapsule::Scene _curr_scene;

	Raylib::Shader _dof_shader;
	Raylib::Shader shader_horizontal_gaussian_blur;
	Raylib::Shader shader_vertical_gaussian_blur;
	int shader_horizontal_gaussian_blur_loc_texture;
	int shader_horizontal_gaussian_blur_loc_weight;
	int shader_vertical_gaussian_blur_loc_texture;
	int shader_vertical_gaussian_blur_loc_weight;

	game_init_t game_init_f = nullptr;
	game_update_t game_update_f = nullptr;
	game_deinit_t game_deinit_f = nullptr;

	std::unordered_map<std::string, highlevel_debug::_debug_perf_entry_t> _debug_perf_entries;
	
	void render(double game_dt);
	
	void game_render_textures_t::unload(void) {
		if(Raylib::IsRenderTextureValid(game_play))
			unload_render_texture_with_depth(game_play);
		if(Raylib::IsRenderTextureValid(world_debug))
			unload_render_texture(world_debug);
		
		for(int i = 0; i < dontchange_game_play_downscaled_len; i++) {
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

	int game_render_textures_t::resize(int game_w, int game_h, int debug_w, int debug_h) {
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

			if(
				Raylib::IsRenderTextureValid(game_play) == false
				|| Raylib::IsRenderTextureValid(world_debug) == false
			) {
				unload();
				return -1;
			}

			game_play_downscaled[0] = load_render_texture(game_w >> 1, game_h >> 1);
			game_play_downscaled[1] = load_render_texture(game_w >> 2, game_h >> 2);
			game_play_downscaled[2] = load_render_texture(game_w >> 2, game_h >> 2);
			
			for(int i = 0; i < _game_render_textures.dontchange_game_play_downscaled_len; i++) {
				if(Raylib::IsRenderTextureValid(game_play_downscaled[i]) == false) {
					unload();
					return -1;
				}

				Raylib::SetTextureFilter(game_play_downscaled[i].texture, Raylib::TEXTURE_FILTER_BILINEAR);
				Raylib::SetTextureWrap(game_play_downscaled[i].texture, Raylib::TEXTURE_WRAP_CLAMP);
			};

			Raylib::SetTextureFilter(game_play.texture, Raylib::TEXTURE_FILTER_BILINEAR);
			Raylib::SetTextureWrap(game_play.texture, Raylib::TEXTURE_WRAP_CLAMP);
		}

		return 0;
	};
	
	[[noreturn]] void init(int argc, const char **argv, game_init_t game_init, game_update_t game_update, game_deinit_t game_deinit) {
		game_init_f = game_init;
		game_update_f = game_update;
		game_deinit_f = game_deinit;

		if(
			game_init == nullptr
			|| game_update == nullptr
			|| game_deinit == nullptr
		) {
			std::fprintf(stderr, "Invalid game functions were provided.\n");
			deinit();
			std::exit(1);
		}

		timecapsule::config_t conf = timecapsule::read_config_from_args(argc, argv);
		(void)conf;
		
		Raylib::SetTargetFPS(60);

		Raylib::SetConfigFlags(Raylib::FLAG_WINDOW_RESIZABLE);
		
		Raylib::InitWindow(640, 360, "TimeCapsule");
		while(Raylib::IsWindowReady() == false) {};
		
		Raylib::SetExitKey(Raylib::KEY_NULL);
		
		Raylib::InitAudioDevice();
		while(Raylib::IsAudioDeviceReady() == false) {};

		scaled_time = 0.0;
		scaled_delta_time = 0.0;
		unscaled_delta_time = 0.0;
		time_scale = 1.0;
		
		if(timecapsule::init_resources() < 0) {
			deinit();
			std::exit(1);
		}

		if(timecapsule::lighting::init() < 0) {
			deinit();
			std::exit(1);
		}

		if(_game_render_textures.resize(16, 16, 2, 2) < 0) {
			std::fprintf(stderr, "[ENGINE] Error: Couldn't load initial render textures\n");
			deinit();
			std::exit(1);
		}

		Raylib::rlExtrasInit();

		_curr_scene.deinit();
		
		_player_camera.current.position = _player_camera.goal.position = Raylib::Vector3 { 0.f, 0.6f, 1.5f };
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
			std::fprintf(stderr, "[ENGINE] Error: Couldn't load load or compile depth-of-field shader.\n");
			deinit();
			std::exit(1);
		}

		shader_horizontal_gaussian_blur = Raylib::LoadShader(nullptr, "./resources/shaders/gaussian-blur-horizontal.fs");
		if(Raylib::IsShaderValid(shader_horizontal_gaussian_blur) == false) {
			std::fprintf(stderr, "[ENGINE] Error: Couldn't load load or compile (Horizontal) Gaussian blur shader.\n");
			deinit();
			std::exit(1);
		}

		shader_vertical_gaussian_blur = Raylib::LoadShader(nullptr, "./resources/shaders/gaussian-blur-vertical.fs");
		if(Raylib::IsShaderValid(shader_vertical_gaussian_blur) == false) {
			std::fprintf(stderr, "[ENGINE] Error: Couldn't load load or compile (Vertical) Gaussian blur shader.\n");
			deinit();
			std::exit(1);
		}

		shader_horizontal_gaussian_blur_loc_texture = Raylib::GetShaderLocation(shader_horizontal_gaussian_blur, "sourceImage");
		shader_horizontal_gaussian_blur_loc_weight = Raylib::GetShaderLocation(shader_horizontal_gaussian_blur, "weight");
		shader_vertical_gaussian_blur_loc_texture = Raylib::GetShaderLocation(shader_horizontal_gaussian_blur, "sourceImage");
		shader_vertical_gaussian_blur_loc_weight = Raylib::GetShaderLocation(shader_vertical_gaussian_blur, "weight");
		
		if(game_init() < 0) {
			deinit();
			std::exit(1);
		}

		bool is_game_input_admin = true;

		unscaled_time = Raylib::GetTime();
		double t0 = Raylib::GetTime();
		
		while(true) {
			unscaled_delta_time = Raylib::GetTime() - t0;
			t0 = Raylib::GetTime();

			scaled_delta_time = unscaled_delta_time * time_scale;
			scaled_time += scaled_delta_time;

			if(timecapsule_should_render_debug_screen) {
				if(engine_internals::is_key_pressed(Raylib::KEY_K))
					is_game_input_admin = is_game_input_admin == false;

				engine_internals::block_game_input(is_game_input_admin == false);
			}

			double t_game = Raylib::GetTime();
			int rc = game_update();

			if(rc < 0)
				break;

			if(engine_internals::is_key_down(Raylib::KEY_F3) && engine_internals::is_key_pressed(Raylib::KEY_R)) {
				if(_recording_perf_enabled) {
					_recording_perf_frame_time /= (double)_recording_perf_frame_number;

					std::printf("[DEBUG] Recorded performance:\n");
					std::printf("  * Time: %f\n", _recording_perf_time);
					std::printf("  * Frames: %u\n", _recording_perf_frame_number);
					std::printf("  * Average frame time: %f (~%f FPS)\n", _recording_perf_frame_time, 1.0 / _recording_perf_frame_time);
					std::printf("  * Score: %f%%\n", ((1.0 / 61.0) / _recording_perf_frame_time) * 100.0);
				} else {
					std::printf("[DEBUG] Recording performance... Press (F3 + R) to stop.\n");

					_recording_perf_time = 0.0;
					_recording_perf_frame_number = 0u;
					_recording_perf_frame_time = 0.0;
				}

				_recording_perf_enabled = _recording_perf_enabled == false;
			}

			render(Raylib::GetTime() - t_game);
		};
		
		deinit();
		std::exit(0);
	};

	float gaussian_weights[9];
	float gaussian_sigma = 2.f;
	
	void blur_game_scene_texture(void) {
		{
			if(engine_internals::is_key_down(Raylib::KEY_UP))
				gaussian_sigma += get_unscaled_delta_time();
			if(engine_internals::is_key_down(Raylib::KEY_DOWN))
				gaussian_sigma -= get_unscaled_delta_time();

			if(gaussian_sigma < 0.00000001f) gaussian_sigma = 0.00000001f;
			if(gaussian_sigma > 2.5f) gaussian_sigma = 2.5f;

			float s2 = gaussian_sigma * gaussian_sigma * 2.f;
			float acc = 0.f;

			for(int x = -4; x <= 4; x++) {
				float w = exp(-((float)x * (float)x) / s2);
				if(x >= 0) gaussian_weights[x] = w;
				acc += w;
			};

			gaussian_weights[0] /= acc;
			gaussian_weights[1] /= acc;
			gaussian_weights[2] /= acc;
			gaussian_weights[3] /= acc;
			gaussian_weights[4] /= acc;
		}

		Raylib::BeginTextureMode(_game_render_textures.game_play_downscaled[0]);
			Raylib::DrawTexturePro(
				_game_render_textures.game_play.texture,
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_game_render_textures.game_play.texture.width,
					(float)_game_render_textures.game_play.texture.height,
				},
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_game_render_textures.game_play_downscaled[0].texture.width,
					(float)_game_render_textures.game_play_downscaled[0].texture.height,
				},
				Raylib::Vector2 { 0.f, 0.f },
				0.f,
				Raylib::WHITE
			);
		Raylib::EndTextureMode();

		Raylib::BeginTextureMode(_game_render_textures.game_play_downscaled[1]);
			Raylib::DrawTexturePro(
				_game_render_textures.game_play_downscaled[0].texture,
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_game_render_textures.game_play_downscaled[0].texture.width,
					(float)_game_render_textures.game_play_downscaled[0].texture.height,
				},
				Raylib::Rectangle {
					0.f,
					0.f,
					(float)_game_render_textures.game_play_downscaled[1].texture.width,
					(float)_game_render_textures.game_play_downscaled[1].texture.height,
				},
				Raylib::Vector2 { 0.f, 0.f },
				0.f,
				Raylib::WHITE
			);
		Raylib::EndTextureMode();

		Raylib::BeginTextureMode(_game_render_textures.game_play_downscaled[2]);
			Raylib::BeginShaderMode(shader_horizontal_gaussian_blur);
				Raylib::SetShaderValueTexture(shader_horizontal_gaussian_blur, shader_horizontal_gaussian_blur_loc_texture, _game_render_textures.game_play_downscaled[1].texture);
				Raylib::SetShaderValueV(shader_horizontal_gaussian_blur, shader_horizontal_gaussian_blur_loc_weight, gaussian_weights, Raylib::SHADER_UNIFORM_FLOAT, 5);
				Raylib::DrawTexturePro(
					_game_render_textures.game_play_downscaled[1].texture,
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play_downscaled[1].texture.width,
						(float)_game_render_textures.game_play_downscaled[1].texture.height,
					},
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play_downscaled[2].texture.width,
						(float)_game_render_textures.game_play_downscaled[2].texture.height,
					},
					Raylib::Vector2 { 0.f, 0.f },
					0.f,
					Raylib::WHITE
				);
			Raylib::EndShaderMode();
		Raylib::EndTextureMode();

		Raylib::BeginTextureMode(_game_render_textures.game_play_downscaled[1]);
			Raylib::BeginShaderMode(shader_vertical_gaussian_blur);
				Raylib::SetShaderValueTexture(shader_vertical_gaussian_blur, shader_vertical_gaussian_blur_loc_texture, _game_render_textures.game_play_downscaled[2].texture);
				Raylib::SetShaderValueV(shader_vertical_gaussian_blur, shader_horizontal_gaussian_blur_loc_weight, gaussian_weights, Raylib::SHADER_UNIFORM_FLOAT, 5);
				Raylib::DrawTexturePro(
					_game_render_textures.game_play_downscaled[2].texture,
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play_downscaled[2].texture.width,
						(float)_game_render_textures.game_play_downscaled[2].texture.height,
					},
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play_downscaled[1].texture.width,
						(float)_game_render_textures.game_play_downscaled[1].texture.height,
					},
					Raylib::Vector2 { 0.f, 0.f },
					0.f,
					Raylib::WHITE
				);
			Raylib::EndShaderMode();
		Raylib::EndTextureMode();

		if(false) {
			Raylib::BeginTextureMode(_game_render_textures.game_play_downscaled[0]);
				Raylib::DrawTexturePro(
					_game_render_textures.game_play_downscaled[1].texture,
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play_downscaled[1].texture.width,
						(float)_game_render_textures.game_play_downscaled[1].texture.height,
					},
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play_downscaled[0].texture.width,
						(float)_game_render_textures.game_play_downscaled[0].texture.height,
					},
					Raylib::Vector2 { 0.f, 0.f },
					0.f,
					Raylib::WHITE
				);
			Raylib::EndTextureMode();
			
			Raylib::BeginTextureMode(_game_render_textures.game_play_downscaled[0]);
				Raylib::DrawTexturePro(
					_game_render_textures.game_play_downscaled[0].texture,
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play_downscaled[0].texture.width,
						(float)_game_render_textures.game_play_downscaled[0].texture.height,
					},
					Raylib::Rectangle {
						0.f,
						0.f,
						(float)_game_render_textures.game_play_downscaled[0].texture.width,
						(float)_game_render_textures.game_play_downscaled[0].texture.height,
					},
					Raylib::Vector2 { 0.f, 0.f },
					0.f,
					Raylib::WHITE
				);
			Raylib::EndTextureMode();
		}
	}
	
	void render(double game_dt) {
		int _debug_perf_entries_count = 0;

		#define PERF_START_RECORD(s)	\
			_debug_perf_entries.insert_or_assign(s, highlevel_debug::_debug_perf_entry_t {	\
				0.0,	\
				_debug_perf_entries_count++,	\
				Raylib::rlBeginQuery(),	\
				0u,	\
			});
		
		#define PERF_STOP_RECORD(s)	\
			if(_debug_perf_entries.find(s) != _debug_perf_entries.cend())	\
				_debug_perf_entries.at(s).gl_query_id_stop = Raylib::rlBeginQuery();
		
		Raylib::BeginDrawing();

		int screen_width = Raylib::GetScreenWidth();
		int screen_height = Raylib::GetScreenHeight();
		
		int game_main_screen_width = screen_width * 0.60f;
		int game_main_screen_height = screen_height;

		if(game_main_screen_width < 16) game_main_screen_width = 16;
		if(game_main_screen_height < 16) game_main_screen_height = 16;

		// Game screen must be at least 16x16.
		if(game_main_screen_width < 16 || game_main_screen_height < 16) {
			std::printf("[DEBUG] Game internal resolution is too small (%ix%i), rendering aborted.\n", screen_width, screen_height);
			
			if(int(Raylib::GetTime()) & 0x1)
				Raylib::ClearBackground(Raylib::Color { 0xff, 0x00, 0x00, 0xff });
			
			Raylib::EndDrawing();
			return;
		}

		// Enforce 16x16 resolution.
		game_main_screen_width >>= 4; game_main_screen_width <<= 4;
		game_main_screen_height >>= 4; game_main_screen_height <<= 4;

		int game_main_debug_screen_width = screen_width * 0.39f;
		int game_main_debug_screen_height = screen_height;

		Raylib::ClearBackground(Raylib::BLACK);

		PERF_START_RECORD("resolution change");

		if(
			_game_render_textures.resize(
				game_main_screen_width,
				game_main_screen_height,
				timecapsule_should_render_debug_screen ? game_main_debug_screen_width : 1,
				timecapsule_should_render_debug_screen ? game_main_debug_screen_height : 1
			) < 0
		) {
			PERF_STOP_RECORD("resolution change");
			
			std::fprintf(stderr, "[ENGINE] Error: Couldn't load resized render textures\n");
			deinit();
			std::exit(1);
		}
		
		PERF_STOP_RECORD("resolution change");
		PERF_START_RECORD("player render");

		unsigned int player_render_triangle_count;
		unsigned int debug_render_triangle_count = 0;

		_curr_scene.render(_player_camera, _game_render_textures.game_play, true);
		player_render_triangle_count = _curr_scene.get_sent_triangles_count();
		
		PERF_STOP_RECORD("player render");

		if(timecapsule_should_render_debug_screen) {
			PERF_START_RECORD("debug render");
			
			// set 'update_cull' to 'false' if you want to watch culling in action.
			// set it to 'true' if you just want to see the actual world itself and optimized too.
			_curr_scene.render(_debug_camera, _game_render_textures.world_debug, false);

			debug_render_triangle_count = _curr_scene.get_sent_triangles_count();

			Raylib::BeginTextureMode(_game_render_textures.world_debug);
			Raylib::BeginMode3D(_debug_camera.current);
			
			highlevel_debug::draw_camera_frustrum_in_3d(_player_camera, _debug_camera.current.position, _game_render_textures.game_play.texture);

			Raylib::EndMode3D();
			Raylib::EndTextureMode();
			
			PERF_STOP_RECORD("debug render");
		}

		PERF_START_RECORD("blur");
			blur_game_scene_texture();
		PERF_STOP_RECORD("blur");

		PERF_START_RECORD("screen");

		Raylib::BeginShaderMode(_dof_shader);
			Raylib::Texture2D blurry_texture = _game_render_textures.game_play_downscaled[1].texture;

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

		PERF_STOP_RECORD("screen");

		if(timecapsule_should_render_debug_screen) {
			PERF_START_RECORD("editor");

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

			PERF_STOP_RECORD("editor");
		}

		for(std::pair<std::string, highlevel_debug::_debug_perf_entry_t> e : _debug_perf_entries) {
			while(Raylib::rlIsQueryResultReady(e.second.gl_query_id_start) == false) {};
			unsigned long start = Raylib::rlGetQueryResultI64(e.second.gl_query_id_start);

			while(Raylib::rlIsQueryResultReady(e.second.gl_query_id_stop) == false) {};
			unsigned long stop = Raylib::rlGetQueryResultI64(e.second.gl_query_id_stop);

			_debug_perf_entries.at(e.first).time = double(stop - start) / 1000000000.0;
		};

		highlevel_debug::draw_debug_page(highlevel_debug::perf_data_t {
			_debug_perf_entries,
			player_render_triangle_count,
			debug_render_triangle_count,
			&_game_render_textures
		});

		if(_recording_perf_enabled) {
			_recording_perf_frame_number++;
			_recording_perf_time += get_unscaled_delta_time();

			for(std::pair<std::string, highlevel_debug::_debug_perf_entry_t> e : _debug_perf_entries)
				_recording_perf_frame_time += e.second.time;
		}

		Raylib::rlClearQueries();

		Raylib::EndDrawing();
	};
	
	void deinit(void) {
		Raylib::EnableCursor();
		
		if(game_deinit_f != nullptr)
			game_deinit_f();

		_curr_scene.deinit();
		Raylib::rlExtrasDeinit();
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
		if(engine_internals::is_key_pressed(Raylib::KEY_P)) _what_thingy_should_i_update = _what_thingy_should_i_update == false;
		return _what_thingy_should_i_update ? _debug_camera : _player_camera;
	};

	double get_time(void) {
		return scaled_time;
	};

	double get_unscaled_time(void) {
		return Raylib::GetTime() - unscaled_time;
	};

	double get_delta_time(void) {
		return scaled_delta_time;
	};

	double get_unscaled_delta_time(void) {
		return unscaled_delta_time;
	};

	double get_time_scale(void) {
		return time_scale;
	};

	void set_time_scale(double factor) {
		time_scale = factor;
	};
};

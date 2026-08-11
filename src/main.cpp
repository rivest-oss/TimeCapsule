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

#include "io/log.hpp"
#include "misc/rtconfig.hpp"
#include "Ox/include/nuclei.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>
#include "misc/asset.hpp"
#include "misc/gui.hpp"
#include "math.hpp"

namespace Raylib {
	#include <raylib.h>
};

void update_and_draw(void);
void deinit(void);

Raylib::Camera3D player_camera;

Raylib::Font debug_font;

const char *FONT_FILENAMES[] = {
	"/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf",
	nullptr,
};

const int debug_models_length = 144;

const char *DEBUG_MODELS_FILENAMES[debug_models_length] = {
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0000-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0001-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0002-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0003-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0004-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0005-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0006-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0007-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0008-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0009-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0010-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0011-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0012-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0013-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0014-8.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-0.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-1.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-2.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-3.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-4.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-5.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-6.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-7.png",
	"resources/partial-heightmap-lods-experiment/downscale-image-0015-8.png",
};

Raylib::Model debug_models[debug_models_length];

const Raylib::Vector3 debug_model_ref_size {
	//1024.f,
	//100.f,
	//1024.f,
	//100.f,
	//10.f,
	//100.f,

	16.f,
	10.f,
	16.f,
};

enum : int {
	GAME_STATE_INVALID = 0,
	GAME_STATE_EXIT,
	GAME_STATE_RUNNING,
	GAME_STATE_PAUSED,
};

int game_state = GAME_STATE_INVALID;

void transition_game_state_to(int new_state) {
	if(new_state == GAME_STATE_EXIT) {
		game_state = GAME_STATE_EXIT;
		deinit();
		std::exit(0);
	}
	
	if(game_state == new_state)
		return;
	
	if(new_state == GAME_STATE_RUNNING) {
		Raylib::DisableCursor();
	}
	
	if(new_state == GAME_STATE_PAUSED) {
		Raylib::EnableCursor();
	}
	
	game_state = new_state;
};

float clampf(float x, float min = -1.f, float max = 1.f) {
	if(x < min) return min;
	if(x > max) return max;
	return x;
};

float vdistance(float a, float b) {
	return std::sqrt(std::pow(b - a, 2.f));
};
float vdistance(Raylib::Vector2 a, Raylib::Vector2 b) {
	return std::sqrt(
		std::pow(b.x - a.x, 2.f)
		+ std::pow(b.y - a.y, 2.f)
	);
};
float vdistance(Raylib::Vector3 a, Raylib::Vector3 b) {
	return std::sqrt(
		std::pow(b.x - a.x, 2.f)
		+ std::pow(b.y - a.y, 2.f)
		+ std::pow(b.z - a.z, 2.f)
	);
};

[[noreturn]] void init(int argc, const char **argv) {
	Ox::Error err;
	
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
	
	player_camera.position = Raylib::Vector3 { 0.f, 2.f, 4.f };
	player_camera.target = Raylib::Vector3 { 0.f, 0.f, 0.f };
	player_camera.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
	player_camera.fovy = 45.f;
	player_camera.projection = Raylib::CAMERA_PERSPECTIVE;
	
	for(int i = 0; FONT_FILENAMES[i] != nullptr; i++) {
		debug_font = timecapsule::load_font(FONT_FILENAMES[i]);
		if(Raylib::IsFontValid(debug_font))
			break;
	};
	
	if(Raylib::IsFontValid(debug_font) == false)
		debug_font = Raylib::GetFontDefault();

	for(int i = 0; i < debug_models_length; i++) {
		Raylib::Image img = timecapsule::load_image(DEBUG_MODELS_FILENAMES[i]);
		if(Raylib::IsImageValid(img) == false) {
			std::fprintf(stderr, "Couldn't load heightmap image: %s\n", DEBUG_MODELS_FILENAMES[i]);
			deinit();
			std::exit(1);
		}

		Raylib::Texture2D diffuse_tex = timecapsule::load_texture_from_image(img);
		if(Raylib::IsTextureValid(diffuse_tex) == false) {
			std::fprintf(stderr, "Couldn't load heightmap diffuse texture: %s\n", DEBUG_MODELS_FILENAMES[i]);
			deinit();
			std::exit(1);
		}
		
		Raylib::Mesh mesh = Raylib::GenMeshHeightmap(img, debug_model_ref_size);
		debug_models[i] = timecapsule::load_model_from_mesh(mesh);

		timecapsule::unload_image(img);

		if(Raylib::IsModelValid(debug_models[i]) == false) {
			std::fprintf(stderr, "Couldn't load heightmap model: %s\n", DEBUG_MODELS_FILENAMES[i]);
			deinit();
			std::exit(1);
		}

		debug_models[i] = timecapsule::update_model_diffuse_texture(debug_models[i], diffuse_tex);
		if(Raylib::IsModelValid(debug_models[i]) == false) {
			std::fprintf(stderr, "Couldn't load heightmap diffuse textue to model: %s\n", DEBUG_MODELS_FILENAMES[i]);
			deinit();
			std::exit(1);
		}
	};
	
	transition_game_state_to(GAME_STATE_PAUSED);
	
	while(true) {
		if(Raylib::WindowShouldClose())
			transition_game_state_to(GAME_STATE_EXIT);
		
		if(Raylib::IsKeyPressed(Raylib::KEY_ESCAPE)) {
			if(game_state == GAME_STATE_RUNNING)
				transition_game_state_to(GAME_STATE_PAUSED);
			
			else if(game_state == GAME_STATE_PAUSED)
				transition_game_state_to(GAME_STATE_RUNNING);
		}
		
		update_and_draw();
	};
	
	transition_game_state_to(GAME_STATE_EXIT);
};

void deinit(void) {
	Raylib::EnableCursor();
	
	timecapsule::deinit_resources();
	
	Raylib::CloseAudioDevice();
	Raylib::CloseWindow();
};

bool is_camera_third_person = false;

void update(double dt) {
	(void)dt;

	const float speed_walking = 1.4f;
	const float speed_running = 4.4f;

	float forward_movement_speed = speed_running;
	float upward_movement_speed = 4.f;

	if(Raylib::IsKeyDown(Raylib::KEY_LEFT_SHIFT)) {
		forward_movement_speed *= 2.f;
		upward_movement_speed *= 4.f;
	}
	
	if(game_state != GAME_STATE_RUNNING)
		return;

	if(Raylib::IsKeyPressed(Raylib::KEY_P))
		is_camera_third_person = is_camera_third_person == false;

	if(is_camera_third_person == false) {
		Raylib::UpdateCameraPro(
			&player_camera,
			Raylib::Vector3 {
				Raylib::IsKeyDown(Raylib::KEY_W) * forward_movement_speed * (float)dt
				- Raylib::IsKeyDown(Raylib::KEY_S) * forward_movement_speed * (float)dt,
				Raylib::IsKeyDown(Raylib::KEY_D) * forward_movement_speed * (float)dt
				- Raylib::IsKeyDown(Raylib::KEY_A) * forward_movement_speed * (float)dt,
				Raylib::GetMouseWheelMove() * upward_movement_speed * (float)dt,
			},
			Raylib::Vector3 {
				Raylib::GetMouseDelta().x * M_PI2f * (float)dt,
				Raylib::GetMouseDelta().y * M_PI2f * (float)dt,
				0.f,
			},
			0.f
		);
	} else {
		Raylib::UpdateCamera(&player_camera, Raylib::CAMERA_THIRD_PERSON);
	}
};

void debug_draw_text(const char *text, int x, int y, int font_height, Raylib::Color text_color, Raylib::Color contourn_color = Raylib::BLANK) {
	for(int sy = -1; sy < 2; sy++)
		for(int sx = -1; sx < 2; sx++)
			if(sy != 0 || sx != 0)
				Raylib::DrawTextEx(
					debug_font,
					text,
					Raylib::Vector2 {
						(float)x + sx,
						(float)y + sy,
					},
					(float)font_height,
					0.1f,
					contourn_color
				);
	
	Raylib::DrawTextEx(
		debug_font,
		text,
		Raylib::Vector2 {
			(float)x,
			(float)y,
		},
		(float)font_height,
		0.1f,
		text_color
	);
};

void debug_draw_rotation(Raylib::Font font, float angle, Raylib::Vector2 screen_pos, float screen_radius) {
	Raylib::DrawCircleLines(screen_pos.x + screen_radius, screen_pos.y + screen_radius, screen_radius, Raylib::WHITE);

	Raylib::DrawLine(
		screen_pos.x + screen_radius,
		screen_pos.y + screen_radius,
		screen_pos.x + screen_radius + std::cos(angle) * screen_radius,
		screen_pos.y + screen_radius + -1.f * std::sin(angle) * screen_radius,
		Raylib::WHITE
	);

	Raylib::DrawCircle(screen_pos.x + screen_radius, screen_pos.y + screen_radius, std::min(4.f, std::max(1.f, screen_radius / 10.f)), Raylib::ColorAlpha(Raylib::WHITE, 0.5f));
	
	int deg = int(angle * (180.f / M_PIf));
	if(deg < 0) deg = 360 - (deg * -1);
	
	Raylib::DrawTextEx(
		Raylib::IsFontValid(font) ? font : Raylib::GetFontDefault(),
		Raylib::TextFormat(
			"%i°",
			deg
		),
		Raylib::Vector2 {
			(float)screen_pos.x + screen_radius * 1.9f,
			(float)screen_pos.y,
		},
		std::min(24.f, std::max(10.f, screen_radius / 2.3f)),
		0.f,
		Raylib::WHITE
	);
};

void debug_draw_rotation_3d(Raylib::Vector3 angle, Raylib::Vector2 screen_pos, float screen_radius, Raylib::Font font = { 0 }) {
	debug_draw_rotation(
		font,
		angle.x,
		Raylib::Vector2 {
			screen_pos.x,
			screen_pos.y,
		},
		screen_radius
	);

	debug_draw_rotation(
		font,
		angle.y,
		Raylib::Vector2 {
			screen_pos.x + screen_radius * 2.5f,
			screen_pos.y,
		},
		screen_radius
	);
};

void debug_draw_camera_info(int base_x, int base_y) {
	Raylib::DrawRectangle(
		base_x,
		base_y,
		300 - 10,
		200 - 10,
		Raylib::ColorAlpha(Raylib::DARKGRAY, 0.5f)
	);

	int font_height = 16;
	int x = base_x + 10, y = base_y + 10;

	debug_draw_text(
		Raylib::TextFormat(
			"Time: %f",
			Raylib::GetTime()
		),
		x,
		y,
		font_height,
		Raylib::WHITE
	);

	y += font_height + 4;

	debug_draw_text(
		Raylib::TextFormat(
			"FPS: %i",
			Raylib::GetFPS()
		),
		x,
		y,
		font_height,
		Raylib::WHITE
	);

	y += font_height + 4;

	debug_draw_text("Camera:", x, y, font_height + 4, Raylib::WHITE);
	y += 8 + font_height;

	debug_draw_text(
		Raylib::TextFormat(
			"Position: %.4f, %.4f, %.4f (XYZ)",
			player_camera.position.x,
			player_camera.position.y,
			player_camera.position.z
		),
		x,
		y,
		font_height,
		Raylib::WHITE
	);

	y += font_height + 4;

	debug_draw_text(
		Raylib::TextFormat(
			"Target: %.4f, %.4f, %.4f (XYZ)",
			player_camera.target.x,
			player_camera.target.y,
			player_camera.target.z
		),
		x,
		y,
		font_height,
		Raylib::WHITE
	);

	y += font_height + 4;

	Raylib::Vector3 camera_diff {
		player_camera.target.x - player_camera.position.x,
		player_camera.target.y - player_camera.position.y,
		player_camera.target.z - player_camera.position.z,
	};

	float camera_length = std::sqrt(
		camera_diff.x * camera_diff.x
		+ camera_diff.y * camera_diff.y
		+ camera_diff.z * camera_diff.z
	);

	float camera_yaw = std::atan2(camera_diff.x, camera_diff.z);
	float camera_pitch = std::asin(camera_diff.y / camera_length);

	debug_draw_rotation_3d(
		Raylib::Vector3 {
			camera_yaw,
			camera_pitch,
			0.f,
		},
		Raylib::Vector2 {
			(float)x,
			(float)y,
		},
		font_height * 2.f,
		debug_font
	);
};

void draw(double dt) {
	Raylib::BeginDrawing();
	Raylib::ClearBackground(Raylib::BLACK);
	
	(void)dt;
	
	int screen_width = Raylib::GetScreenWidth();
	int screen_height = Raylib::GetScreenHeight();
	
	if(true) {
		debug_draw_camera_info(screen_width - 300, 10);
		
		if(game_state == GAME_STATE_PAUSED) {
			for(int y = -1; y < 2; y++)
				for(int x = -1; x < 2; x++)
					if(x != 0 && y != 0)
					Raylib::DrawText("Game paused", x + 16, y + 16, 32, Raylib::BLACK);
			
			Raylib::DrawText("Game paused", 16, 16, 32, Raylib::WHITE);
		}
	}

	Raylib::BeginMode3D(player_camera);

	Raylib::DrawSphereWires(
		Raylib::Vector3 {
			0.f,
			0.f,
			0.f,
		},
		1.f,
		5,
		10,
		Raylib::GREEN
	);

	Raylib::Color colors[9] = {
		Raylib::ColorLerp(Raylib::GREEN, Raylib::YELLOW, 0.f),
		Raylib::ColorLerp(Raylib::GREEN, Raylib::YELLOW, 1.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::GREEN, Raylib::YELLOW, 2.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::GREEN, Raylib::YELLOW,3.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::YELLOW, Raylib::RED, 1.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::YELLOW, Raylib::RED, 2.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::YELLOW, Raylib::RED,3.f*(1.f/3.f)),
		Raylib::RED,
		Raylib::PURPLE,
	};

	const unsigned int lod_levels = 9;

	for(int z = 0; z < 4; z++) {
		for(int x = 0; x < 4; x++) {
			int i = (z * 4 + x) * lod_levels;

			Raylib::Vector3 curr_model_pos {
				debug_model_ref_size.x * x,
				-(debug_model_ref_size.y),
				debug_model_ref_size.z * z,
			};

			Raylib::Vector3 curr_model_pos_clamped {
				clampf(
					player_camera.position.x,
					curr_model_pos.x,
					curr_model_pos.x + debug_model_ref_size.x
				),
				curr_model_pos.y,
				clampf(
					player_camera.position.z,
					curr_model_pos.z,
					curr_model_pos.z + debug_model_ref_size.z
				),
			};

			float dist = vdistance(player_camera.position, curr_model_pos_clamped);

			int lod_number = 0;
			if(dist > 0.0 && std::isnan(dist) == false && std::isfinite(dist))
				//lod_number = dist < 0.0001f ? 0 : int(dist / 10.f);
				lod_number = dist < 0.0001f ? 0 : int(dist / debug_model_ref_size.x);
			
			if(lod_number > 7)
				lod_number = 7;

			Raylib::Color point_colors[4] = {
				Raylib::RED,
				Raylib::GREEN,
				Raylib::BLUE,
				Raylib::PURPLE,
			};

			Raylib::DrawSphere(
				Raylib::Vector3 {
					curr_model_pos_clamped.x,
					curr_model_pos_clamped.y + 10.f,
					curr_model_pos_clamped.z,
				},
				0.1f,
				point_colors[((x & 0x1) << 1) | (z & 0x1)]
			);

			Raylib::DrawLine3D(
				Raylib::Vector3 {
					curr_model_pos_clamped.x,
					curr_model_pos_clamped.y - 10.f,
					curr_model_pos_clamped.z,
				},
				Raylib::Vector3 {
					curr_model_pos_clamped.x,
					curr_model_pos_clamped.y + 10.f,
					curr_model_pos_clamped.z,
				},
				point_colors[((x & 0x1) << 1) | (z & 0x1)]
			);

			Raylib::DrawModel(
				debug_models[i + lod_number],
				curr_model_pos,
				1.f,
				colors[lod_number]
			);
		};
	};

	if(is_camera_third_person) {
		Raylib::DrawCapsule(
			Raylib::Vector3 {
				player_camera.target.x,
				player_camera.target.y - 1.50f,
				player_camera.target.z,
			},
			Raylib::Vector3 {
				player_camera.target.x,
				player_camera.target.y,
				player_camera.target.z,
			},
			0.50f,
			16,
			16,
			Raylib::DARKBLUE
		);
	}

	Raylib::EndMode3D();
	
	Raylib::EndDrawing();
	Raylib::SwapScreenBuffer();
};

void update_and_draw(void) {
	double dt = Raylib::GetFrameTime();
	update(dt);
	draw(dt);
};

int main(int argc, const char **argv) {
	init(argc, argv);
	return 0;
};
